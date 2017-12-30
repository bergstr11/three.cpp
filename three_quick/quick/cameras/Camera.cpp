//
// Created by byter on 12/14/17.
//

#include <quick/scene/Scene.h>

namespace three {
namespace quick {

three::Camera::Ptr Camera::create()
{
  _camera = _create();

  _camera->position().set(_position.x(), _position.y(), _position.z());

  auto *scene = dynamic_cast<Scene *>(_lookAt);
  if(scene) {
    _camera->lookAt(scene->scene()->position());
  }
  else {
    auto *object = dynamic_cast<ThreeQObject *>(_lookAt);
    if(object) {
      _camera->lookAt(object->object()->position());
    }
  }
  return _camera;
}

}
}