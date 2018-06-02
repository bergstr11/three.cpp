//
// Created by byter on 3/16/18.
//

#include <vector>
#include <QMouseEvent>
#include <QQmlEngine>
#include <threepp/quick/ThreeDItem.h>
#include <threepp/quick/scene/Scene.h>
#include <threepp/util/Resolver.h>
#include <threepp/quick/materials/MeshBasicMaterial.h>
#include <threepp/quick/materials/MeshPhongMaterial.h>
#include <threepp/quick/materials/MeshLambertMaterial.h>
#include <threepp/quick/materials/ShaderMaterial.h>
#include <threepp/core/Raycaster.h>
#include <threepp/math/Triangle.h>
#include "ObjectPicker.h"

namespace three {
namespace quick {

struct no_delete
{
  template <typename T>
  void operator () (const T *o) const {}
} _no_delete;

using namespace std;
using namespace math;

void SingleRay::setIntersects(std::vector<Intersection> &intersects)
{
  //only sorty by distance
  std::sort(intersects.begin(), intersects.end(),
            [](const Intersection &a, const Intersection &b) {return a.distance < b.distance;});

  if(!intersects.empty()) _picked = intersects[0].object;
}

Raycaster SingleRay::raycaster(const Ray &cameraRay)
{
  _origin = cameraRay.origin();
  _surfaceNormal = cameraRay.direction().negated();
  return Raycaster(cameraRay);
}

float normal_distance(const Intersection &intersect)
{
  //Gegenkathete b = c * sin(beta)
  float b = intersect.distance * sin(intersect.direction.angleTo(intersect.face.normal));

  //Ankathete a = sqrt(c*c - b*b)
  return sqrt(pow(intersect.distance, 2.0f) - pow(b, 2.0f));
}

Object3D *calculateSurface(std::vector<Intersection> &intersects,
                           unsigned segments, Vector3 &position, Vector3 &normal)
{
  //find the bundle with the nearest point
  float min_dist = intersects[0].distance;
  Object3D *obj = intersects[0].object;
  unsigned bundleStart = 0, bundlePos = 0, minPos = 0;

  for(unsigned i=1; i<intersects.size(); i++) {
    if(intersects[i].object != obj) {
      bundleStart = i;
    }
    if(intersects[i].distance < min_dist) {
      min_dist = intersects[i].distance;
      minPos = i;
      bundlePos = bundleStart;
    }
  }

  Object3D *picked = intersects[minPos].object;

  //count the number of hits for this bundle
  unsigned bundleSize = minPos - bundlePos;
  for(unsigned i=minPos; i<intersects.size() && intersects[i].object == picked; i++) {
    bundleSize++;
  }

  //if(bundleSize < 3) {
    //not enough data. Use normal at minpos
    position = intersects[minPos].point;
    normal = intersects[minPos].face.normal;

    return picked;
  //}
#if 0
  Ray ray(intersects[minPos].point, intersects[minPos].face.normal);
  Vector3 dir = ray.direction();
  float angle1 = numeric_limits<float>::max();
  float angle2 = numeric_limits<float>::max();
  unsigned pos1, pos2;

  unsigned mid = (float)bundleSize / 2;

  for(unsigned i1=minPos-1, i2=minPos+1; mid > 0; mid--) {
    ray.lookAt(intersects[i1].point);

    float angl = ray.direction().angleTo(dir);
    if(angl < angle1) {
      angle1 = angl;
      pos1 = i1;
    }

    ray.lookAt(intersects[i2].point);

    angl = ray.direction().angleTo(dir);
    if(angl < angle2) {
      angle2 = angl;
      pos2 = i2;
    }

    i1 = i1 == 0 ? bundlePos + bundleSize - 1 : i1 - 1;
    i2++;
  }

  Triangle t(intersects[minPos].point, intersects[pos1].point, intersects[pos2].point);

  normal = t.getNormal();
  position = t.getMidpoint();

  qDebug() << "surface:" << position.x() << position.y() << position.z() << "||" << normal.x() << normal.y() << normal.z();
  return picked;
#endif
}

void CircularRays::setIntersects(std::vector<Intersection> &intersects)
{
  _picked = calculateSurface(intersects, _segments, _surfacePosition, _surfaceNormal);
}

Raycaster CircularRays::raycaster(const Ray &cameraRay)
{
  _origin = cameraRay.origin();
  return Raycaster::circular(cameraRay, _radius, _segments);
}

bool ObjectPicker::handleMousePressed(QMouseEvent *event)
{
  if(_lastX == event->x() && _lastY == event->y())  return false;
  _lastX = event->x();
  _lastY = event->y();

  if(_camera && _item && event->button() == Qt::LeftButton) {

    float x = ((float)event->x() / (float)_item->width()) * 2 - 1;
    float y = -((float)event->y() / (float)_item->height()) * 2 + 1;

    const Ray cameraRay = _camera->camera()->ray(x, y);
    Raycaster raycaster = _rays->raycaster(cameraRay);

    _intersects.clear();
    _currentIntersect.object.clear();

    for (const auto &obj : _objects) {
      Object3D::Ptr o3d;

      ThreeQObject *to = obj.value<ThreeQObject *>();
      if(to) o3d = to->object();
      else {
        Pickable * po = obj.value<Pickable *>();
        if(po) o3d = po->object();
      }
      if(!o3d) continue;

      three::intersectObject( *o3d, raycaster, _intersects, true);
      if(!_intersects.empty()) {
        _currentIntersect.object = obj;
        _currentIntersect.set(_intersects[0]);

        break;
      }
    }
    if(_scene && _intersects.empty()) {
      for (auto obj : _scene->scene()->children()) {
        three::intersectObject( *obj, raycaster, _intersects, true );
      }
    }
    if(!_intersects.empty()) {
      _rays->setIntersects(_intersects);

      emit objectsClicked();
      return  true;
    }
    else {
      for(const auto &picker : _pickers) {
        if(picker->enabled() && picker->handleMousePressed(event)) return true;
      }
    }
  }
  return false;
}

QVariant ObjectPicker::intersect(unsigned index)
{
  if(_intersects.size() > index) {
    if(!_prototype) {
      _prototype = new ThreeQObject();
    }
    shared_ptr<Object3D> obj(const_cast<Object3D *>(_intersects[index].object), _no_delete);
    if(obj != _prototype->object()) {

      _prototype->setObject(obj);
      if(_prototype->material()) _prototype->material()->deleteLater();

      if(obj->material()) {

        if(CAST(obj->material(), mat, three::MeshPhongMaterial)) {
          _prototype->setMaterial(new MeshPhongMaterial(mat));
        }
        else if(CAST(obj->material(), mat, three::MeshLambertMaterial)) {
          _prototype->setMaterial(new MeshLambertMaterial(mat));
        }
        else if(CAST(obj->material(), mat, three::MeshBasicMaterial)) {
          _prototype->setMaterial(new MeshBasicMaterial(mat));
        }
      }
    }

    _currentIntersect.object.setValue(_prototype);
    _currentIntersect.set(_intersects[index]);

    QVariant var;
    var.setValue(&_currentIntersect);
    return var;
  }
  else if(index == 0 && !_currentIntersect.object.isNull()){
    //_currentIntersect already set
    QVariant var;
    var.setValue(&_currentIntersect);
    return var;
  }
  throw std::out_of_range("intersect");
}

bool ObjectPicker::handleMouseDoubleClicked(QMouseEvent *event)
{
  if(!_currentIntersect.object.isNull()) {
    emit objectsDoubleClicked();
    return true;
  }
  else if(!_intersects.empty()) {
    emit objectsDoubleClicked();
    return true;
  }
  else {
    for(const auto &picker : _pickers) {
      if(picker->handleMouseDoubleClicked(event)) return true;
    }
  }
  return false;
}

ObjectPicker::ObjectPicker(QObject *parent)
   : ThreeQObjectRoot(parent), _currentIntersect(this), _prototype(nullptr), _rays(new SingleRay())
{
  QQmlEngine::setObjectOwnership(&_currentIntersect, QQmlEngine::CppOwnership);
}

ObjectPicker::~ObjectPicker() {
  if(_prototype) _prototype->deleteLater();
}

void ObjectPicker::setItem(ThreeDItem *item)
{
  if(_item != item) {
    _item = item;
    Interactor::setItem(item);
  }

  for(const auto &picker : _pickers) {
    picker->setItem(item);
  }
}

void ObjectPicker::setRays(Rays *rays)
{
  if(_rays != rays) {
    if(_rays) _rays->deleteLater();

    _rays = rays;
    emit raysChanged();
  }
}

void ObjectPicker::setCamera(Camera *camera)
{
  if(_camera != camera) {
    _camera = camera;
    emit cameraChanged();
  }
}

void ObjectPicker::setObjects(const QVariantList &objects)
{
  _objects.clear();
  _scene = nullptr;

  for(const QVariant &var : objects) {
    _scene = var.value<Scene *>();
    if(!_scene) {
      _objects.push_back(var);
    }
  }
}

void ObjectPicker::append_picker(QQmlListProperty<ObjectPicker> *list, ObjectPicker *obj)
{
  ObjectPicker *item = qobject_cast<ObjectPicker *>(list->object);
  if (item) item->_pickers.append(obj);
}
int ObjectPicker::count_pickers(QQmlListProperty<ObjectPicker> *list)
{
  ObjectPicker *item = qobject_cast<ObjectPicker *>(list->object);
  return item ? item->_pickers.size() : 0;
}
ObjectPicker *ObjectPicker::picker_at(QQmlListProperty<ObjectPicker> *list, int index)
{
  ObjectPicker *item = qobject_cast<ObjectPicker *>(list->object);
  return item ? item->_pickers.at(index) : nullptr;
}
void ObjectPicker::clear_pickers(QQmlListProperty<ObjectPicker> *list)
{
  ObjectPicker *item = qobject_cast<ObjectPicker *>(list->object);
  if(item) item->_pickers.clear();
}

QQmlListProperty<ObjectPicker> ObjectPicker::pickers()
{
  return QQmlListProperty<ObjectPicker>(this, nullptr,
                                        &ObjectPicker::append_picker,
                                        &ObjectPicker::count_pickers,
                                        &ObjectPicker::picker_at,
                                        &ObjectPicker::clear_pickers);
}

}
}
