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

void SingleRay::setIntersects(std::vector<Intersection> &intersects)
{
  //only sorty by distance
  std::sort(intersects.begin(), intersects.end(),
            [](const Intersection &a, const Intersection &b) {return a.distance < b.distance;});
}

Raycaster SingleRay::raycaster(const math::Ray &cameraRay)
{
  _origin = cameraRay.origin();
  _surfaceNormal = cameraRay.direction().negated();
  return Raycaster(cameraRay);
}

math::Vector3 calculateSurfaceNormal(std::vector<Intersection> &intersects)
{
  float first=numeric_limits<unsigned>::max(), second=numeric_limits<unsigned>::max(), third=numeric_limits<unsigned>::max();
  unsigned findex, sindex, tindex;

  for(unsigned i=0; i<intersects.size(); i++) {
    float dist = intersects[i].distance;

    if(dist < first) {
      first = dist;
      findex = i;
    }
    if(dist >= first && dist < second) {
      second = dist;
      sindex = i;
    }
    if(dist >= first && dist >= second && dist < third) {
      third = dist;
      tindex = i;
    }
  }

  return math::Triangle::normal(intersects[findex].point, intersects[sindex].point, intersects[tindex].point);
}

void CircularRays::setIntersects(std::vector<Intersection> &intersects)
{
  _surfaceNormal = calculateSurfaceNormal(intersects);
}

Raycaster CircularRays::raycaster(const math::Ray &cameraRay)
{
  _origin = cameraRay.origin();
  return Raycaster(cameraRay.origin(), Raycaster::createCircularBundle(cameraRay, _radius, _segments));
}

Raycaster SquareRays::raycaster(const math::Ray &cameraRay)
{
  _origin = cameraRay.origin();
  return Raycaster(cameraRay.origin(), Raycaster::createSquareBundle(cameraRay, _sideLength, _sideSegments));
}

void SquareRays::setIntersects(std::vector<Intersection> &intersects)
{
  _surfaceNormal = calculateSurfaceNormal(intersects);
}

bool ObjectPicker::handleMousePressed(QMouseEvent *event)
{
  if(_lastX == event->x() && _lastY == event->y())  return false;
  _lastX = event->x();
  _lastY = event->y();

  if(_camera && _item && event->button() == Qt::LeftButton) {

    float x = ((float)event->x() / (float)_item->width()) * 2 - 1;
    float y = -((float)event->y() / (float)_item->height()) * 2 + 1;

    const math::Ray cameraRay = _camera->camera()->ray(x, y);
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

      three::intersectObject( *o3d, raycaster, _intersects, true, _rays->pickFirst() );
      if(_rays->pickFirst() && !_intersects.empty()) {
        _currentIntersect.object = obj;
        _currentIntersect.set(_intersects[0]);

        break;
      }
    }
    if(_scene && (!_rays->pickFirst()  || _intersects.empty())) {
      for (auto obj : _scene->scene()->children()) {
        three::intersectObject( *obj, raycaster, _intersects, true, _rays->pickFirst() );
        if(_rays->pickFirst() && !_intersects.empty()) break;
      }
    }
    if(!_intersects.empty()) {
      _rays->setIntersects(_intersects);

      emit objectsClicked();
      return  true;
    }
    else {
      for(const auto &picker : _pickers) {
        if(picker->handleMousePressed(event)) return true;
      }
    }
  }
  return false;
}

QVariant ObjectPicker::intersect(unsigned index)
{
  if(_intersects.size() > index) {
    if(_prototype) {
      shared_ptr<Object3D> obj(const_cast<Object3D *>(_intersects[index].object), _no_delete);
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

      _currentIntersect.object.setValue(_prototype);
    }
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
