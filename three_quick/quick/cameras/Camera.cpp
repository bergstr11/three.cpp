//
// Created by byter on 12/14/17.
//

#include <quick/scene/Scene.h>

namespace three {
namespace quick {

three::Camera::Ptr Camera::create(three::Scene::Ptr scene)
{
  _camera = _create();

  if(!_position.isNull())
    _camera->position().set(_position.x(), _position.y(), _position.z());

  if(!_lookAt.isNull())
    _camera->lookAt(math::Vector3(_lookAt.x(), _lookAt.y(), _lookAt.z()));

  return _camera;
}

}
}