//
// Created by byter on 2/9/18.
//

#include "ThreeQObject.h"
#include <threepp/quick/scene/Scene.h>

namespace three {
namespace quick {

VertexNormalsHelper *ThreeQObject::vertexNormals()
{
  if(!_normalsHelper) {
    _normalsHelper = new VertexNormalsHelper(this);

    if(_object) {
      _normalsHelper->create(_object, _parentObject, true);
    }
  }
  return _normalsHelper;
}

three::Object3D::Ptr ThreeQObject::create(Scene *scene, Object3D::Ptr parent)
{
  _scene = scene;
  _parentObject = parent;

  _object = _create();
  if(_object) {
    if(!_rotation.isNull())
      _object->rotation().set(_rotation.x(), _rotation.y(), _rotation.z());

    _object->position().set(_position.x(), _position.y(), _position.z());

    if(!_name.isEmpty())
      _object->setName(_name.toStdString());

    _object->castShadow = _castShadow;
    _object->receiveShadow = _receiveShadow;
    _object->matrixAutoUpdate = _matrixAutoUpdate;
    _object->visible() = _visible;

    for(auto o : _objects) {
      three::Object3D::Ptr obj = o->create(_scene, _object);
      if(obj) _object->add(obj);
    }

    if(_normalsHelper) _normalsHelper->create(_object, _parentObject);
  }
  _post_create();
  return _object;
}

void ThreeQObject::append_object(QQmlListProperty<ThreeQObject> *list, ThreeQObject *obj)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  if (item) item->_objects.append(obj);
}
int ThreeQObject::count_objects(QQmlListProperty<ThreeQObject> *list)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  return item ? item->_objects.size() : 0;
}
ThreeQObject *ThreeQObject::object_at(QQmlListProperty<ThreeQObject> *list, int index)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  return item ? item->_objects.at(index) : nullptr;
}
void ThreeQObject::clear_objects(QQmlListProperty<ThreeQObject> *list)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  if(item) item->_objects.clear();
}

QQmlListProperty<ThreeQObject> ThreeQObject::objects()
{
  return QQmlListProperty<ThreeQObject>(this, nullptr,
                                        &ThreeQObject::append_object,
                                        &ThreeQObject::count_objects,
                                        &ThreeQObject::object_at,
                                        &ThreeQObject::clear_objects);
}

void ThreeQObject::setObject(const three::Object3D::Ptr object)
{
  _object = object;
  const math::Vector3 pos = _object ? _object->position() : math::Vector3();
  setPosition(QVector3D(pos.x(), pos.y(), pos.z()), false);

  const math::Euler rot = _object ? _object->rotation() : math::Euler();
  setRotation(QVector3D(rot.x(), rot.y(), rot.z()), false);

  setName(QString::fromStdString(object->name()), false);
  setCastShadow(object->castShadow, false);
  setReceiveShadow(object->castShadow, false);
  setVisible(object->visible(), false);
  setMatrixAutoUpdate(object->matrixAutoUpdate, false);

  const math::Vector3 &s = object->scale();
  setScale(QVector3D(s.x(), s.y(), s.z()), false);

  if(_normalsHelper) {
    _normalsHelper->setObject(_object, _parentObject);
  }
}

void ThreeQObject::rotateX(float angle)
{
  if(_object) {
    _object->rotateX(angle);
    _rotation.setX(_object->rotation().x());
    emit rotationChanged();
  }
}
void ThreeQObject::rotateY(float angle)
{
  if(_object) {
    _object->rotateY(angle);
    _rotation.setY(_object->rotation().y());
    emit rotationChanged();
  }
}
void ThreeQObject::rotateZ(float angle)
{
  if(_object) {
    _object->rotateZ(angle);
    _rotation.setZ(_object->rotation().z());
    emit rotationChanged();
  }
}

void ThreeQObject::translateZ(float distance)
{
  if(_object) {
    _object->translateZ(distance);
    _position.setZ(_object->position().z());
    emit positionChanged();
  }
}

void ThreeQObject::lookAt(const QVector3D &position)
{
  if(_object) {
    _object->lookAt(math::Vector3(position.x(), position.y(), position.z()));

    const math::Euler rot = _object->rotation();
    setRotation(QVector3D(rot.x(), rot.y(), rot.z()), false);
  }
}

}
}