//
// Created by byter on 12/14/17.
//

#include <quick/scene/Scene.h>

namespace three {
namespace quick {

three::Camera::Ptr Camera::create(three::Scene::Ptr scene)
{
  _camera = _create();

  if(_position.x() != std::numeric_limits<float>::infinity())
    _camera->position().set(_position.x(), _position.y(), _position.z());
  else {
    _position = toQVector3D(_camera->position());
    emit positionChanged();
  }

  if(!_lookAt.isNull())
    _camera->lookAt(math::Vector3(_lookAt.x(), _lookAt.y(), _lookAt.z()));
  else {
    //_lookAt = toQVector3D(_camera->lookAt())
  }

  if(_rotation.x() != std::numeric_limits<float>::infinity())
    _camera->rotation() = _rotation;
  else {
    _rotation = _camera->rotation();
    emit rotationChanged();
  }

  return _camera;
}

}
}