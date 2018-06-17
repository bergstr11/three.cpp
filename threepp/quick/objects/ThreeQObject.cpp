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

BoundingBox *ThreeQObject::boundingBox()
{
  if(!_boundingBox && _object) {
    auto bbox = _object->computeBoundingBox();
    _boundingBox = new BoundingBox(bbox, this);
  }
  return _boundingBox;
}

three::Object3D::Ptr ThreeQObject::create(Scene *scene, Object3D::Ptr parent)
{
  _scene = scene;
  _parentObject = parent;

  Object3D::Ptr prev = _object;
  _object = _create();

  if(_object) {
    if(!_rotation().isNull())
      _object->rotation().set(_rotation().x(), _rotation().y(), _rotation().z());

    _object->position().set(_position().x(), _position().y(), _position().z());

    _object->scale().set(_scale().x(), _scale().y(), _scale().z());

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

    _post_create();

    if(_parentObject) _parentObject->add(_object);

    onObjectChanged.emitSignal(_object, ObjectState::Added);
    _object->updateMatrix();
  }

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
  if(_object) {

    if(_position.isSet()) {
      object->position().set(_position().x(), _position().y(), _position().z());
    }
    else {
      const math::Vector3 pos = _object->position();
      setPosition(QVector3D(pos.x(), pos.y(), pos.z()), false);
      _position.unset();
    }

    if(_rotation.isSet()) {
      object->rotation().set(_rotation().x(), _rotation().y(), _rotation().z());
    }
    else {
      const math::Euler rot = _object->rotation();
      setRotation(QVector3D(rot.x(), rot.y(), rot.z()), false);
      _rotation.unset();
    }

    setName(QString::fromStdString(_object->name()), false);

    if(_castShadow.isSet()) _object->castShadow = _castShadow;
    else setCastShadow(_object->castShadow, false);

    if(_receiveShadow.isSet()) _object->receiveShadow = _receiveShadow;
    else setReceiveShadow(_object->castShadow, false);

    _object->visible() = _visible;
    _object->matrixAutoUpdate = _matrixAutoUpdate;

    if(_scale.isSet()) {
      _object->scale().set(_scale().x(), _scale().y(), _scale().z());
    }
    else {
      const math::Vector3 &s = _object->scale();
      setScale(QVector3D(s.x(), s.y(), s.z()), false);
      _scale.unset();
    }

    if(_normalsHelper) {
      _normalsHelper->setObject(_object, _parentObject);
    }
  }
}

void ThreeQObject::unset()
{
  _position.unset();
  _rotation.unset();
  _castShadow.unset();
  _receiveShadow.unset();
  _scale.unset();
}

void ThreeQObject::rotateX(float angle)
{
  if(_object) {
    _object->rotateX(angle);
    _rotation().setX(_object->rotation().x());
    emit rotationChanged();
  }
}
void ThreeQObject::rotateY(float angle)
{
  if(_object) {
    _object->rotateY(angle);
    _rotation().setY(_object->rotation().y());
    emit rotationChanged();
  }
}
void ThreeQObject::rotateZ(float angle)
{
  if(_object) {
    _object->rotateZ(angle);
    _rotation().setZ(_object->rotation().z());
    emit rotationChanged();
  }
}

void ThreeQObject::translateZ(float distance)
{
  if(_object) {
    _object->translateZ(distance);
    _position().setZ(_object->position().z());
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