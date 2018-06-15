//
// Created by byter on 3/16/18.
//

#include <vector>
#include <algorithm>

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

using namespace std;
using namespace math;

struct no_delete
{
  template <typename T>
  void operator () (const T *o) const {}
} _no_delete;

void SingleRay::setIntersects(IntersectList &intersects)
{
  if(!intersects.empty()) _picked = intersects.get(0, 0).object;
}

Raycaster SingleRay::raycaster(const Ray &cameraRay)
{
  _origin = cameraRay.origin();
  _surfaceNormal = cameraRay.direction().negated();
  return Raycaster(cameraRay);
}

void CircularRays::setSegments(unsigned segments)
{
  if(segments % 3) {
    qWarning() << "CircularRays: segments should be multiple of 3";
    if(segments < 3) segments = 3;
  }
  if(segments != _segments) {
    _segments = segments;
    emit segmentsChanged();
  }
}

void CircularRays::setIntersects(IntersectList &intersects)
{
  _picked = intersects.calculateSurface(_surfacePosition, _surfaceNormal);
}

Raycaster CircularRays::raycaster(const Ray &cameraRay)
{
  _origin = cameraRay.origin();

  float scaled = _scaleTo / 100.0f * _radius;
  return Raycaster::circular(cameraRay, scaled, _segments);
}

void ObjectPicker::scaleTo(ThreeQObject *object)
{
  if(object && object->object()) {
    math::Box3 bbox = object->object()->computeBoundingBox();

    _scaleSize = bbox.getBoundingSphere().radius() * 2;
    if(_rays) _rays->scaleTo(_scaleSize);
  }
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

      if(_currentIntersect.object.isNull())
        _currentIntersect.object = obj;

      raycaster.intersectObject( *o3d, _intersects, true);
    }
    if(_scene && _intersects.empty()) {
      //if scene was given, and no objects given or intersected, try scene's children
      for (auto obj : _scene->scene()->children()) {
        raycaster.intersectObject( *obj, _intersects, true );
      }
    }
    if(!_intersects.empty() && _rays->accept(_intersects)) {

      _currentIntersect.set(_intersects.get(0, 0));

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
  if(index < _intersects.rayCount()) {
    if(!_prototype) {
      _prototype = new ThreeQObject();
    }
    shared_ptr<Object3D> obj(const_cast<Object3D *>(_intersects.get(index, 0).object), _no_delete);
    if(obj != _prototype->object()) {

      _prototype->unset();
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
    _currentIntersect.set(_intersects.get(index, 0));

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
  if(!_currentIntersect.object.isNull() || !_intersects.empty()) {
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
    _rays->scaleTo(_scaleSize);
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
