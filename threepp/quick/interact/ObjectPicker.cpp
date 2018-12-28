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
#include <threepp/quick/materials/MeshStandardMaterial.h>
#include <threepp/quick/materials/ShaderMaterial.h>
#include <threepp/core/Raycaster.h>
#include <threepp/math/Triangle.h>
#include "ObjectPicker.h"

namespace three {
namespace quick {

using namespace std;
using namespace math;

void SingleRay::setIntersects(IntersectList &intersects)
{
  if(!intersects.empty()) {
    Intersection &is = intersects.get(0, 0);
    _picked = is.object;
    _surfacePosition = is.point;
    _surfaceNormal = is.face.normal;
  }
}

Raycaster SingleRay::raycaster(const Ray &cameraRay)
{
  _origin = cameraRay.origin();
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

void ObjectPicker::findIntersects(float ex, float ey)
{
  float x = (ex / (float)_item->width()) * 2 - 1;
  float y = -(ey / (float)_item->height()) * 2 + 1;

  const Ray cameraRay = _camera->camera()->ray(x, y);
  Raycaster raycaster = _rays->raycaster(cameraRay);

  _intersects.clear();
  _currentIntersect.object.clear();

  for (const auto &obj : _objects) {
    Object3D::Ptr o3d;

    auto *to = obj.value<ThreeQObject *>();
    if(to) o3d = to->object();
    else {
      auto * po = obj.value<Pickable *>();
      if(po) o3d = po->object();
    }
    if(!o3d) continue;

    raycaster.intersectObject( *o3d, _intersects, true);
  }
  if(_scene && _intersects.empty()) {
    //if scene was given, and no objects given or intersected, try scene's children
    for (auto obj : _scene->scene()->children()) {
      raycaster.intersectObject( *obj, _intersects, true );
    }
  }
  if(!_intersects.empty()) _intersects.prepare();
  /*qDebug() << "== intersects:";
  for(const auto &intersect : _intersects) {
    qDebug() << intersect.object->name().c_str() << intersect.distance << intersect.object->parent()->name().c_str();
  }*/
}

bool ObjectPicker::handleMouseReleased(QMouseEvent *event)
{
  return handleMouseClicked(event);
}

bool ObjectPicker::handleMouseClicked(QMouseEvent *event)
{
  _mouseX = event->x();
  _mouseY = event->y();

  if(_camera && _item && event->button() == Qt::LeftButton) {
    findIntersects(event->x(), event->y());

    if(!_intersects.empty() && _rays->accept(_intersects)) {

      _rays->setIntersects(_intersects);

      emit objectPicked();
      return  true;
    }
    else {
      for(const auto &picker : _pickers) {
        if(picker->enabled() && picker->handleMouseClicked(event))
          return true;
      }
    }
  }
  return false;
}

bool ObjectPicker::handleMouseDoubleClicked(QMouseEvent *event)
{
  if(!_unifyClicked) return false;

  _mouseX = event->x();
  _mouseY = event->y();

  if(_camera && _item && event->button() == Qt::LeftButton) {
    findIntersects(event->x(), event->y());

    if(!_intersects.empty() && _rays->accept(_intersects)) {

      _rays->setIntersects(_intersects);

      emit objectDoublePicked();
      return true;
    }
    else {
      for(const auto &picker : _pickers) {
        if(picker->handleMouseDoubleClicked(event)) return true;
      }
    }
  }
  return false;
}

QVariant ObjectPicker::intersect(unsigned index)
{
  if(_intersects.empty() || _intersects.count(0) <= index)
    throw std::out_of_range("intersect");

  if(!_accessObject) _accessObject = new ThreeQObject();

  Object3D::Ptr obj(_intersects.get(0, index).object, _no_delete);
  if(obj != _accessObject->object()) {

    _accessObject->unset();
    _accessObject->setObject(obj);

    if(_accessMaterial) {
      _accessMaterial->deleteLater();
      _accessMaterial = nullptr;
    }

    if(obj->material()) {

      if(CAST(obj->material(), mat, three::MeshPhongMaterial)) {
        _accessMaterial = new MeshPhongMaterial(mat);
        _accessObject->setMaterial(_accessMaterial, false);
      }
      else if(CAST(obj->material(), mat, three::MeshLambertMaterial)) {
        _accessMaterial = new MeshLambertMaterial(mat);
        _accessObject->setMaterial(_accessMaterial, false);
      }
      else if(CAST(obj->material(), mat, three::MeshBasicMaterial)) {
        _accessMaterial = new MeshBasicMaterial(mat);
        _accessObject->setMaterial(_accessMaterial, false);
      }
      else if(CAST(obj->material(), mat, three::MeshStandardMaterial)) {
        _accessMaterial = new MeshStandardMaterial(mat);
        _accessObject->setMaterial(_accessMaterial, false);
      }
      else {
        _accessObject->setMaterial(nullptr, false);
      }
    }
    else
      _accessObject->setMaterial(nullptr, false);
  }

  _currentIntersect.object.setValue(_accessObject);
  _currentIntersect.set(_intersects.get(0, index));

  QVariant var;
  var.setValue(&_currentIntersect);
  return var;
}

QStringList ObjectPicker::pickedParents(unsigned index)
{
  QStringList names;

  if(!_intersects.empty() && _intersects.count(0) > index) {
    Object3D *o = _intersects.get(0, index).object;

    while(o) {
      QString nm = !o->name().empty() ? QString::fromStdString(o->name()) : QString::number(o->id());
      names << nm;
      o = o->parent();
    }
  }
  return names;
}

ObjectPicker::ObjectPicker(QObject *parent)
   : ThreeQObjectRoot(parent), _currentIntersect(this), _accessObject(nullptr), _rays(new SingleRay())
{
  QQmlEngine::setObjectOwnership(&_currentIntersect, QQmlEngine::CppOwnership);
}

ObjectPicker::~ObjectPicker() {
  if(_accessObject) _accessObject->deleteLater();
  if(_rays) delete _rays;
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

void ObjectPicker::setUnifyClicked(bool unify)
{
  if(_unifyClicked != unify) {
    _unifyClicked = unify;
    emit unifyClickedChanged();
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
