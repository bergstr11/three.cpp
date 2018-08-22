//
// Created by byter on 10.09.17.
//
#include "impl/raycast.h"
#include <threepp/math/Circle3.h>

namespace three {

using namespace std;
using namespace math;

/*
 * helper class that groups an originating point on the ray bundle perimeter together with
 * target perimeter points that can be reached collision-free. Target points are collapsed
 * while their predecessor is collision-free, thus building a triangle if two or more points
 * are involved.
 */
class RingPos
{
  vector<array<Vector3, 2>> _targets;

  bool _open = false;

public:
  Object3D * picked;
  Vector3 origin;
  float distance;

  RingPos(const Intersection &intersect)
     : picked(intersect.object), origin(intersect.point), distance(intersect.distance) {}

  //add another intersection point
  void setCurrent(const Intersection &is) {
    if(_open) {
      _targets.back()[1] = is.point;
    }
    else {
      _open = true;
      _targets.push_back({is.point, Vector3()});
    }
  }

  //a collision was detected, close the target group
  void closeCurrent() {_open = false;}

  //return truie if there are no collision free target points
  bool empty() const {return _targets.empty();}

  //true if this RinPos contains at least one target group with 2 points
  bool hasTriangle() const
  {
    for(const auto &target : _targets) {
      if(!target[1].isNull()) return true;
    }
    return false;
  }

  //return the longest collision-free line
  Line3 getMaxLine() const
  {
    Line3 max;
    for(const auto &target : _targets) {

      float dist = origin.distanceTo(target[0]);
      if(dist > max.distance()) max = Line3(origin, target[0]);

      if(!target[1].isNull()) {
        dist = origin.distanceTo(target[1]);
        if(dist > max.distance()) max = Line3(origin, target[1]);
      }
    }
    return max;
  }

  //return the collision-free triangle with the largest area
  Triangle getMaxTriangle() const
  {
    Triangle max;
    for(const auto &target : _targets) {
      if(!target[1].isNull()) {
        Triangle tri(origin, target[0], target[1]);

        if(tri.getArea() > max.getArea()) max = tri;
      }
    }
    return max;
  }

  //return the target point
  Vector3 first() const {return _targets[0][0];}
};

inline size_t ringpos(unsigned rayCount, size_t pos)
{
  return pos - (pos / rayCount * rayCount);
}

/*
 * calculate the ideal resting surface for a planar marker
 */
Object3D *IntersectList::calculateSurface(Vector3 &position, Vector3 &normal)
{
  Raycaster raycaster;
  vector<RingPos> positions;

  Object3D *object = nullptr;

  const Intersection &center = get(0, 0);

  const Intersection &one = get(1, 0);
  const Intersection &two = get(5, 0);
  const Intersection &three = get(9, 0);

  //let every hit point in the ray bundle find the siblings he can see collision-free
  for(unsigned pos=1, rayCount = this->rayCount(); pos < rayCount; pos++) {

    const Intersection &is = get(pos, 0);

    positions.emplace_back(is);
    RingPos &ringPos = positions.back();
    IntersectList collisions;

    for(unsigned npos = ringpos(rayCount, pos+1); npos != pos; npos = ringpos(rayCount, npos+1)) {

      if(npos == 0) continue;
      const Intersection &nis = get(npos, 0);

      Vector3 direction = (nis.point - ringPos.origin).normalized();
      float distance = ringPos.origin.distanceTo(nis.point);

      raycaster.set(Ray(ringPos.origin, direction));
      nis.object->raycast(raycaster, collisions);

      if(!collisions.hasIntersects(distance))
        ringPos.setCurrent(nis);

      else
        ringPos.closeCurrent();
    }

    if(ringPos.empty())
      //no collision-free sight, revert
      positions.pop_back();
  }

  if(!positions.empty()) {

    std::sort(positions.begin(), positions.end(), [](const RingPos &a, const RingPos &b) {
      return a.distance < b.distance;
    });

    const RingPos &ringPos = positions.front();
    if(ringPos.hasTriangle()) {
      Triangle tria = ringPos.getMaxTriangle();

      //set the in/out parameter values
      object = ringPos.picked;
      position = tria.getMidpoint();
      normal = object->worldToLocal(tria.getNormal());
    }
    else {
      Line3 line = ringPos.getMaxLine();

      //set the in/out parameter values
      object = ringPos.picked;
      position = line.getCenter();
      normal = center.face.normal;
    }
  }
  else {
    //out of luck
    object = center.object;
    position = center.point;
    normal = center.face.normal;
  }

  return object;
}

void intersectObject(Object3D &object,
                     const Raycaster &raycaster,
                     IntersectList &intersects,
                     bool recursive )
{
  if (!object.visible()) return;

  object.raycast( raycaster, intersects );

  if (recursive ) {
    const std::vector<Object3D::Ptr> &children = object.children();

    for (auto child : children) {
      intersectObject(*child, raycaster, intersects, recursive);
    }
  }
}

void IntersectList::prepare()
{
  //throw out empty ray bins
  for(auto iter = _intersections.end() - 1; iter >= _intersections.begin();) {
    auto erase = iter;
    iter--;
    if(erase->empty()) _intersections.erase(erase);
  }

  //sort each ray's bin so the closest intersection comes first
  for(auto &intersects : _intersections) {
    std::sort(intersects.begin(), intersects.end(), [this](const Intersection &a, const Intersection &b) {
      return a.distance < b.distance;
    });
  }
}

void Raycaster::intersectObject(Object3D &object, IntersectList &intersects, bool recursive ) const
{
  three::intersectObject( object, *this, intersects, recursive );

  if(!intersects.empty()) intersects.prepare();
}

void Raycaster::intersectObjects(const std::vector<Object3D::Ptr> objects,
                                 IntersectList &intersects,
                                 bool recursive ) const
{
  for (auto obj : objects) {
    three::intersectObject( *obj, *this, intersects, recursive );
  }
  if(!intersects.empty()) intersects.prepare();
}

std::vector<math::Ray> Raycaster::createCircularBundle(
   const math::Ray &ray, float radius, unsigned radialSegments)
{
  std::vector<math::Ray> rays;

  //center ray first
  rays.push_back(ray);

  math::Circle3 circle(ray.origin(), ray.direction(), radius);

  //then the circle rays
  float delta = 2.0f * (float)M_PI / radialSegments;
  for (unsigned seg = 0; seg < radialSegments; seg++ ) {

    float theta = (float)seg * delta;
    Vertex p = circle.pointAt(theta);

    rays.emplace_back(p, ray.direction());
  }
  return rays;
}

IntersectList::iterator::iterator(IntersectList &list, bool end)
   : _list(list),
     _index(end ? list._intersections.size() : 0),
     _begin(end ? std::vector<Intersection>::iterator() : list._intersections[_index].begin()),
     _end(end)
{
}

Intersection &IntersectList::iterator::operator*() const
{
  return *_begin;
}

IntersectList::iterator & IntersectList::iterator::operator++()
{
  if(_begin == _list._intersections[_index].end()) {
    _index++;
    _end = _index >= _list._intersections.size();
    if(!_end) _begin = _list._intersections[_index].begin();
  }
  else {
    _begin++;
    _end = _begin == _list._intersections[_index].end() && _index + 1 == _list._intersections.size();
  }
  return *this;
}

IntersectList::iterator &IntersectList::iterator::operator++(int i)
{
  return ++(*this);
}

bool IntersectList::iterator::operator ==(const iterator & rhs) const
{
	return _begin == rhs._begin || (_end && rhs._end);
}

bool IntersectList::iterator::operator !=(const iterator & rhs) const
{
  return !operator ==(rhs);
}

IntersectList::iterator IntersectList::begin()
{
  return {*this, _intersections.empty()};
}

IntersectList::iterator IntersectList::end()
{
  return {*this, true};
}

}
