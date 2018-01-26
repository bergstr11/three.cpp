//
// Created by byter on 12/14/17.
//

#include <quick/scene/Scene.h>

namespace three {
namespace quick {

const float Camera::infinity = std::numeric_limits<float>::infinity();

void Camera::_post_create(Scene *scene)
{
  if(_lookAt.x() != infinity) {
    _camera->lookAt(math::Vector3(_lookAt.x(), _lookAt.y(), _lookAt.z()));
    updateControllerValues();
  }
}

three::Object3D::Ptr Camera::_create(Scene *scene)
{
  _camera = _createCamera();

  return _camera;
}

}
}
