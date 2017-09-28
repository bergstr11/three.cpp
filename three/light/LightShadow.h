//
// Created by byter on 14.09.17.
//

#ifndef THREE_QT_LIGHTSHADOW_H
#define THREE_QT_LIGHTSHADOW_H

#include <camera/Camera.h>
#include <math/Vector2.h>
#include <textures/Texture.h>
#include <type_traits>
#include <renderers/Renderer.h>
#include "Light.h"

namespace three {

class Light;

class LightShadow
{
protected:
  float _bias = 0;
  float _radius = 1;

  math::Vector2 _mapSize {512, 512};

  Renderer::Target::Ptr _map;
  math::Matrix4 _matrix;

public:
  using Ptr = std::shared_ptr<LightShadow>;

  virtual void update(std::shared_ptr<Light> light) {}

  float bias() const {return _bias;}
  float radius() const {return _radius;}
  const math::Vector2 &mapSize() const {return _mapSize;}

  const Renderer::Target::Ptr &map() const {return _map;}
  Renderer::Target::Ptr &map() {return _map;}

  math::Matrix4 &matrix() {return _matrix;}

  virtual const Camera::Ptr camera() const {return nullptr;}
};


template <typename C=Camera>
class CameraShadow : public LightShadow
{
  static_assert(std::is_base_of<Camera, C>::value, "template parameter must be derived from three::Camera");

protected:
  std::shared_ptr<C> _camera;

  explicit CameraShadow(std::shared_ptr<C> camera) : _camera(camera) {}

public:
  using Ptr = std::shared_ptr<CameraShadow<C>>;
  static Ptr make(std::shared_ptr<C> camera) {
    return Ptr(new CameraShadow(camera));
  }

  const typename std::shared_ptr<C> specificCamera() const {return _camera;}

  const Camera::Ptr camera() const override {return _camera;}
};

}
#endif //THREE_QT_LIGHTSHADOW_H
