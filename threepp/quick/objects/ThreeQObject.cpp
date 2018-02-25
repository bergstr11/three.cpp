//
// Created by byter on 2/9/18.
//

#include "ThreeQObject.h"

namespace three {
namespace quick {

three::Object3D::Ptr ThreeQObject::create(Scene *scene)
{
  _object = _create(scene);
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
  }
  _post_create(scene);
  return _object;
}

void ThreeQObject::setObject(three::Object3D::Ptr object)
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

}
}