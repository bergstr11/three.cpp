//
// Created by byter on 14.09.17.
//

#ifndef THREE_QT_LIGHTSHADOW_H
#define THREE_QT_LIGHTSHADOW_H

#include <camera/PerspectiveCamera.h>
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

  PerspectiveCamera::Ptr _camera;

protected:
  explicit LightShadow(PerspectiveCamera::Ptr camera) : _camera(camera) {}

public:
  using Ptr = std::shared_ptr<LightShadow>;
  static Ptr make(PerspectiveCamera::Ptr camera) {
    return Ptr(new LightShadow(camera));
  }

  float bias() const {return _bias;}
  float radius() const {return _radius;}

  float &bias() {return _bias;}
  float &radius() {return _radius;}

  const math::Vector2 &mapSize() const {return _mapSize;}
  math::Vector2 &mapSize() {return _mapSize;}

  const Renderer::Target::Ptr &map() const {return _map;}
  void setMap(const Renderer::Target::Ptr &map) {_map = map;}

  math::Matrix4 &matrix() {return _matrix;}
  const math::Matrix4 &matrix() const {return _matrix;}

  virtual const PerspectiveCamera::Ptr camera() const {return _camera;}

  virtual void update() {}
};

}
#endif //THREE_QT_LIGHTSHADOW_H
