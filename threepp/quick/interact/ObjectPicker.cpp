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
#include "ObjectPicker.h"

namespace three {
namespace quick {

struct no_delete
{
  template <typename T>
  void operator () (const T *o) const {}
} _no_delete;

using namespace std;

bool ObjectPicker::handleMousePressed(QMouseEvent *event)
{
  if(_lastX == event->x() && _lastY == event->y())  return false;
  _lastX = event->x();
  _lastY = event->y();

  _mouse = nullptr;
  if(_camera && _item && event->button() == Qt::LeftButton) {
    QVector2D m(((float)event->x() / (float)_item->width()) * 2 - 1,
                -((float)event->y() / (float)_item->height()) * 2 + 1);

    _raycaster.set(m);

    size_t prevCount = _intersects.size();
    _intersects.clear();

    for (auto obj : _threeQObjects) {
      three::intersectObject( *obj->object(), _raycaster.raycaster(), _intersects, _recurse );
    }
    if(_scene) {
      for (auto obj : _scene->scene()->children()) {
        three::intersectObject( *obj, _raycaster.raycaster(), _intersects, _recurse );
      }
    }
    std::sort(_intersects.begin(), _intersects.end(),
              [](const Intersection &a, const Intersection &b) {return a.distance < b.distance;});

    if(!_intersects.empty()) {
      _mouse = event;

      if(prevCount != _intersects.size())
         emit intersectCountChanged();
      emit objectsClicked();
    }
  }
  return false;
}

QVariant ObjectPicker::intersect(unsigned index)
{
  if(_intersects.size() > index) {
    shared_ptr<Object3D> obj(const_cast<Object3D *>(_intersects[index].object), _no_delete);
    _currentObject.setObject(obj);
    if(_currentObject.material()) _currentObject.material()->deleteLater();

    if(obj->material()) {
      material::Dispatch dispatch;

      dispatch.func<three::MeshPhongMaterial>() = [&] (three::MeshPhongMaterial &mat) {
        three::MeshPhongMaterial::Ptr m(&mat, _no_delete);
        _currentObject.setMaterial(new MeshPhongMaterial(m));
      };
      dispatch.func<three::MeshLambertMaterial>() = [&] (three::MeshLambertMaterial &mat) {
        three::MeshLambertMaterial::Ptr m(&mat, _no_delete);
        _currentObject.setMaterial(new MeshLambertMaterial(m));
      };
      dispatch.func<three::MeshBasicMaterial>() = [&] (three::MeshBasicMaterial &mat) {
        three::MeshBasicMaterial::Ptr m(&mat, _no_delete);
        _currentObject.setMaterial(new MeshBasicMaterial(m));
      };

      obj->material()->resolver->material::DispatchResolver::getValue(dispatch);
    }
    _currentIntersect.set(&_currentObject, _intersects[index]);
    QVariant var;
    var.setValue(&_currentIntersect);
    return var;
  }
  return QVariant();
}

bool ObjectPicker::handleMouseDoubleClicked(QMouseEvent *event)
{
  if(!_intersects.empty()) {
    _mouse = event;

    emit objectsDoubleClicked();
  }
  return false;
}

ObjectPicker::ObjectPicker(QObject *parent)
   : Controller(parent), _currentIntersect(this), _currentObject(this)
{
  QObject::connect(this, &Controller::cameraChanged, [this]() {
    _raycaster.setCamera(_camera);
  });
  QQmlEngine::setObjectOwnership(&_currentIntersect, QQmlEngine::CppOwnership);
  QQmlEngine::setObjectOwnership(&_currentObject, QQmlEngine::CppOwnership);
}

ObjectPicker::~ObjectPicker() {
  _currentIntersect.deleteLater();
  _currentObject.deleteLater();
}

void ObjectPicker::setItem(ThreeDItem *item)
{
  if(_item != item) {
    _item = item;
    Controller::setItem(item);
  }
}

QVariantList ObjectPicker::objects()
{
  QVariantList objects;

  for(auto obj : _threeQObjects) {
    QVariant var;
    var.setValue(obj);
    objects.push_back(var);
  }
  return objects;
}

void ObjectPicker::setObjects(QVariantList objects)
{
  _threeQObjects.clear();
  _scene = nullptr;

  for(const QVariant &var : objects) {
    ThreeQObject *o = var.value<ThreeQObject *>();
    if(o) _threeQObjects.push_back(o);
    else _scene = var.value<Scene *>();
  }
}

}
}
