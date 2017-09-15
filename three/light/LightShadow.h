//
// Created by byter on 14.09.17.
//

#ifndef THREE_QT_LIGHTSHADOW_H
#define THREE_QT_LIGHTSHADOW_H

#include <camera/Camera.h>
#include <math/Vector2.h>
#include <textures/Texture.h>

namespace three {

class LightShadow
{
  Camera::Ptr _camera;

  float _bias = 0;
  float _radius = 1;

  math::Vector2 _mapSize {512, 512};

  Texture::Ptr _map;
  math::Matrix4 matrix;

  LightShadow(Camera::Ptr camera) : _camera(camera) {}

public:
  using Ptr = std::shared_ptr<LightShadow>;
  static Ptr make(Camera::Ptr camera) {
    return std::shared_ptr<LightShadow>(camera);
  }

  const Camera::Ptr camera() const {return _camera;}

  const math::Vector2 &mapSize() const {return _mapSize;}

  const Texture::Ptr map() const {return _map;}
  void setMap(const Texture::Ptr map) {_map = map;}
};

}
#endif //THREE_QT_LIGHTSHADOW_H
