//
// Created by byter on 10.09.17.
//
#include "impl/raycast.h"
#include <threepp/math/Circle3.h>

namespace three {

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
     _begin(end ? list._intersections.back().end() : list._intersections.front().begin()),
     _end(list._intersections.back().end())
{
}

Intersection &IntersectList::iterator::operator*() const
{
  return *_begin;
}

IntersectList::iterator & IntersectList::iterator::operator++()
{
  if(_begin == _end) {
    _index++;
    _begin = _list._intersections[_index].begin();
    _end = _list._intersections[_index].end();
  }
  else _begin++;
  return *this;
}

IntersectList::iterator &IntersectList::iterator::operator++(int i)
{
  return ++(*this);
}

bool IntersectList::iterator::operator ==(const iterator & rhs) const
{
	return _index == rhs._index && _begin == rhs._begin;
}

bool IntersectList::iterator::operator !=(const iterator & rhs) const
{
  return !operator ==(rhs);
}

IntersectList::iterator IntersectList::begin()
{
  return IntersectList::iterator(*this, 0);
}
IntersectList::iterator IntersectList::end()
{
  return IntersectList::iterator(*this, true);
}

}
