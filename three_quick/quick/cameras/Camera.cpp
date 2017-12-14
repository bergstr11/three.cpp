//
// Created by byter on 12/14/17.
//

#include <ThreeDScene.h>

namespace three {
namespace quick {

three::Camera::Ptr Camera::create()
{
  _camera = _create();

  auto *scene = dynamic_cast<ThreeDScene *>(_lookAt);
  if(scene) {
    _camera->lookAt(scene->scene()->position());
  }
  else {
    auto *object = dynamic_cast<ThreeDObject *>(_lookAt);
    if(object) {
      _camera->lookAt(object->object()->position());
    }
  }
  return _camera;
}

}
}