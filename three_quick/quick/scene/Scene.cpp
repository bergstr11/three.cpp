//
// Created by cse on 5/12/17.
//

#include <functional>

#include <QOpenGLExtraFunctions>
#include <QQuickWindow>
#include <QThread>
#include <QScreen>

#include "Scene.h"
#include "../Three.h"

namespace three {
namespace quick {

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
      _quickCamera->deleteLater();
    }
    _quickCamera = camera;
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

void Scene::setBackground(const QColor &background) {
  if (_background != background) {
    _background = background;
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

void Scene::add(ThreeQObject *object)
{
  if(_scene) {
    auto obj = object->object();
    if(!obj) obj = object->create(this);
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

void Scene::addTo(ObjectRootContainer *container)
{
  _scene = _background.isValid() ?
           three::SceneT<Color>::make(_name.toStdString(), Color(_background.redF(), _background.greenF(), _background.blueF()))
                                 : three::Scene::make(_name.toStdString());

  _position.setX(_scene->position().x());
  _position.setY(_scene->position().y());
  _position.setZ(_scene->position().z());
  positionChanged();

  if(_fog) _scene->fog() = _fog->create();
  _quickCamera->create(this);

  if(_quickCamera->controller())
    container->addController(_quickCamera->controller(), _quickCamera->camera());

  //cameras with children need to be parented to the scene!
  if(!_quickCamera->camera()->children().empty())
    _scene->add(_quickCamera->camera());

  for(auto &object :_objects) {
    auto obj = object->create(this);
    if(obj) _scene->add(obj);
  }
  container->addScene(this);
}

}
}
