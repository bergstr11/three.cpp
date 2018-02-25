//
// Created by byter on 12/14/17.
//

#include "Camera.h"
#include <threepp/quick/scene/Scene.h>

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
  _camera = _createCamera(_near, _far);

  for(auto &light :_lights) {
    auto l = light->create(scene);
    if(l) _camera->add(l);
  }

  return _camera;
}

void Camera::append_light(QQmlListProperty<Light> *list, Light *obj)
{
  Camera *item = qobject_cast<Camera *>(list->object);
  if (item) item->_lights.append(obj);
}
int Camera::count_lights(QQmlListProperty<Light> *list)
{
  Camera *item = qobject_cast<Camera *>(list->object);
  return item ? item->_lights.size() : 0;
}
Light *Camera::light_at(QQmlListProperty<Light> *list, int index)
{
  Camera *item = qobject_cast<Camera *>(list->object);
  return item ? item->_lights.at(index) : nullptr;
}
void Camera::clear_lights(QQmlListProperty<Light> *list)
{
  Camera *item = qobject_cast<Camera *>(list->object);
  if(item) item->_lights.clear();
}


QQmlListProperty<Light> Camera::lights()
{
  return QQmlListProperty<Light>(this, nullptr,
                                        &Camera::append_light,
                                        &Camera::count_lights,
                                        &Camera::light_at,
                                        &Camera::clear_lights);
}

}
}
