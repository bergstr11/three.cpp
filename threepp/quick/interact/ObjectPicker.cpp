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

  if(_camera && _item && event->button() == Qt::LeftButton) {
    QVector2D m(((float)event->x() / (float)_item->width()) * 2 - 1,
                -((float)event->y() / (float)_item->height()) * 2 + 1);

    _raycaster.set(m);

    size_t prevCount = _intersects.size();
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
      if(_pickAll)
        three::intersectObject( *o3d, _raycaster.raycaster(), _intersects, true );
      else {
        std::vector<Intersection> intersects;
        three::intersectObject( *o3d, _raycaster.raycaster(), intersects, true );
        if(!intersects.empty()) {
          _currentIntersect.object = obj;
          _currentIntersect.set(intersects[0]);

          if(prevCount != 1) emit intersectCountChanged();
          emit objectsClicked();
          return  true;
        }
      }
    }
    if(_scene && _pickAll) {
      for (auto obj : _scene->scene()->children()) {
        three::intersectObject( *obj, _raycaster.raycaster(), _intersects, true );
      }
    }
    if(!_intersects.empty()) {
      std::sort(_intersects.begin(), _intersects.end(),
                [](const Intersection &a, const Intersection &b) {return a.distance < b.distance;});

      if(prevCount != _intersects.size()) emit intersectCountChanged();
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
  if(_pickAll) {
    if(_intersects.size() > index) {
      shared_ptr<Object3D> obj(const_cast<Object3D *>(_intersects[index].object), _no_delete);
      _prototype->setObject(obj);
      if(_prototype->material()) _prototype->material()->deleteLater();

      if(obj->material()) {
        material::Dispatch dispatch;

        dispatch.func<three::MeshPhongMaterial>() = [&] (three::MeshPhongMaterial &mat) {
          three::MeshPhongMaterial::Ptr m(&mat, _no_delete);
          _prototype->setMaterial(new MeshPhongMaterial(m));
        };
        dispatch.func<three::MeshLambertMaterial>() = [&] (three::MeshLambertMaterial &mat) {
          three::MeshLambertMaterial::Ptr m(&mat, _no_delete);
          _prototype->setMaterial(new MeshLambertMaterial(m));
        };
        dispatch.func<three::MeshBasicMaterial>() = [&] (three::MeshBasicMaterial &mat) {
          three::MeshBasicMaterial::Ptr m(&mat, _no_delete);
          _prototype->setMaterial(new MeshBasicMaterial(m));
        };

        obj->material()->resolver->material::DispatchResolver::getValue(dispatch);
      }

      _currentIntersect.object.setValue(_prototype);
      _currentIntersect.set(_intersects[index]);

      QVariant var;
      var.setValue(&_currentIntersect);
      return var;
    }
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
  if(_pickAll) {
    if(!_intersects.empty()) {
      emit objectsDoubleClicked();
      return true;
    }
  }
  else if(!_currentIntersect.object.isNull()) {
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
   : Controller(parent), _currentIntersect(this), _prototype(nullptr)
{
  QObject::connect(this, &Controller::cameraChanged, [this]() {
    _raycaster.setCamera(_camera);
  });
  QQmlEngine::setObjectOwnership(&_currentIntersect, QQmlEngine::CppOwnership);
}

ObjectPicker::~ObjectPicker() {
  if(_prototype) _prototype->deleteLater();
}

void ObjectPicker::setItem(ThreeDItem *item)
{
  if(_item != item) {
    _item = item;
    Controller::setItem(item);
  }
  if(_pickAll && !_prototype) {
    _prototype = new ThreeQObject();
    QQmlEngine::setObjectOwnership(_prototype, QQmlEngine::CppOwnership);
  }

  for(const auto &picker : _pickers) {
    picker->setItem(item);
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
