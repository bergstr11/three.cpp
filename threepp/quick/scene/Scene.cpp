//
// Created by cse on 5/12/17.
//

#include <functional>

#include <QOpenGLExtraFunctions>
#include <QQuickWindow>
#include <QScreen>

#include "Scene.h"
#include <threepp/quick/ThreeDItem.h>
#include <threepp/textures/Texture.h>

namespace three {
namespace quick {

Scene::Scene(QObject *parent) : ThreeQObjectRoot(parent) {}

void Scene::setName(const QString &name) {
  if (_name != name) {
    _name = name;
    emit nameChanged();
  }
}

void Scene::setQuickCamera(Camera *camera)
{
  if(_quickCamera != camera) {
    if(_quickCamera) {
      if(_quickCamera->camera()) _scene->remove(_quickCamera->camera());
    }
    _quickCamera = camera;
    if(_scene) updateCamera();

    emit cameraChanged();
  }
}

void Scene::setFog(FogBase *fog)
{
  if(_fog != fog) {
    if(_fog) {
      _fog->deleteLater();
    }
    _fog = fog;
    emit fogChanged();
  }
}

void Scene::updateCamera()
{
  if(!_quickCamera->camera()) _quickCamera->create(this, _scene);

  //cameras with children need to be parented to the scene!
  if(!_quickCamera->camera()->children().empty())
    _scene->add(_quickCamera->camera());
}

void Scene::setBgColor(const QColor &background) {
  if (_bgColor != background) {
    _bgColor = background;
    emit backgroundChanged();
  }
}

void Scene::setBgTexture(Texture *background) {
  if (_bgTexture != background) {
    _bgTexture = background;
    emit backgroundChanged();
  }
}

void Scene::append_object(QQmlListProperty<ThreeQObject> *list, ThreeQObject *obj)
{
  Scene *item = qobject_cast<Scene *>(list->object);
  if (item) item->_objects.append(obj);
}
int Scene::count_objects(QQmlListProperty<ThreeQObject> *list)
{
  Scene *item = qobject_cast<Scene *>(list->object);
  return item ? item->_objects.size() : 0;
}
ThreeQObject *Scene::object_at(QQmlListProperty<ThreeQObject> *list, int index)
{
  Scene *item = qobject_cast<Scene *>(list->object);
  return item ? item->_objects.at(index) : nullptr;
}
void Scene::clear_objects(QQmlListProperty<ThreeQObject> *list)
{
  Scene *item = qobject_cast<Scene *>(list->object);
  if(item) item->_objects.clear();
}

QQmlListProperty<ThreeQObject> Scene::objects()
{
  return QQmlListProperty<ThreeQObject>(this, nullptr,
                                        &Scene::append_object,
                                        &Scene::count_objects,
                                        &Scene::object_at,
                                        &Scene::clear_objects);
}

void updateMatrixAll(Object3D::Ptr object)
{
  object->updateMatrix();
  for(const auto &child : object->children()) {
    updateMatrixAll(child);
  }
}

void Scene::clone(ThreeQObject *object)
{
  if(_scene) {
    auto doClone = [this, object]() {
      auto obj = object->object();
      if(obj) {
        auto obj2 = Object3D::Ptr(obj->cloned());
        _scene->add(obj2);
        updateMatrixAll(_scene);
      }
    };
    if(_item) _item->lockWhile(doClone);
    else doClone();
  }
}

void Scene::add(ThreeQObject *object)
{
  if(_scene) {
    auto obj = object->object();
    if(!obj) obj = object->create(this, _scene);
    if(obj) {
      _scene->add(obj);
      obj->updateMatrix();
    }
  }
}

void Scene::remove(ThreeQObject *object)
{
  if(_scene && object->object()) {
    _scene->remove(object->object());
  }
}

void Scene::clear()
{
  if(_scene) {
    _scene->removeAll();
  }
}

void Scene::setItem(ThreeDItem *item)
{
  _item = item;

  if(_bgTexture) {
    _scene = three::SceneT<three::Texture::Ptr>::make(_name.toStdString(), _bgTexture->getTexture());
  }
  else if(_bgColor.isValid()) {
    _scene = three::SceneT<Color>::make(_name.toStdString(), Color(_bgColor.redF(), _bgColor.greenF(), _bgColor.blueF()));
  }
  else {
    _scene = three::Scene::make(_name.toStdString());
  }

  _position.setX(_scene->position().x());
  _position.setY(_scene->position().y());
  _position.setZ(_scene->position().z());
  positionChanged();

  if(_fog) _scene->fog() = _fog->create();

  if(_quickCamera) updateCamera();

  for(auto &object :_objects) {
    auto obj = object->create(this, _scene);
    if(obj) _scene->add(obj);
  }
  item->addScene(this);
}

}
}
