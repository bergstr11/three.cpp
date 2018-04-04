//
// Created by byter on 14.09.17.
//

#ifndef THREEPP_LIGHTSHADOW_H
#define THREEPP_LIGHTSHADOW_H

#include <type_traits>

#include <threepp/camera/PerspectiveCamera.h>
#include <threepp/math/Vector2.h>
#include <threepp/renderers/Renderer.h>
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

  virtual const Camera::Ptr camera() const = 0;

  virtual void update() {}

  virtual LightShadow *cloned() const = 0;
};

template <typename tCamera>
class LightShadowT : public LightShadow
{
  static_assert(std::is_base_of<three::Camera, tCamera>::value, "tCamera must be a three::Camera subtype");
  friend class LightShadow;
  
protected:
  typename tCamera::Ptr _camera;

  explicit LightShadowT(typename tCamera::Ptr camera) : _camera(camera) {}

public:
  using Ptr = std::shared_ptr<LightShadowT>;
  static Ptr make(std::shared_ptr<tCamera> camera) {
    return Ptr(new LightShadowT(camera));
  }

  const Camera::Ptr camera() const override {return _camera;}

  const typename tCamera::Ptr camera_t() const {return _camera;}

  LightShadowT *cloned() const override {
    return new LightShadowT(*this);
  }
};

}
#endif //THREEPP_LIGHTSHADOW_H
