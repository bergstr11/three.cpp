//
// Created by byter on 2/19/18.
//

#ifndef THREE_PP_DIRECTIONALLIGHT_H
#define THREE_PP_DIRECTIONALLIGHT_H

#include <threepp/light/DirectionalLight.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/material/LineMaterial.h>
#include <threepp/objects/Line.h>

namespace three {
namespace helper {

class DirectionalLight : public Object3D
{
  three::DirectionalLight::Ptr _light;
  Color _color;

  Line::Ptr _lightPlane, _targetLine;

protected:
  DirectionalLight(three::DirectionalLight::Ptr light, float size, const Color &color)
     : Object3D(object::Resolver::makeNull()), _light(light), _color(color)
  {
    _light->updateMatrixWorld(true);

    _matrix = _light->matrixWorld();
    matrixAutoUpdate = false;

    auto geometry = BufferGeometry::make();
    auto position = attribute::prealloc<float, Vertex>(5);
    position->next() = {-size, size, 0};
    position->next() = {size, size, 0};
    position->next() = {size, -size, 0};
    position->next() = {-size, -size, 0};
    position->next() = {-size, size, 0};
    geometry->setPosition(position);

    auto material = LineBasicMaterial::make();
    _lightPlane = Line::make("lightPlane", geometry, material);
    add(_lightPlane);

    geometry = BufferGeometry::make();
    position = attribute::prealloc<float, Vertex>(2);
    position->next() = {0, 0, 0};
    position->next() = {0, 0, 1};
    geometry->setPosition(position);

    _targetLine = Line::make("targetLine", geometry, material);
    add(_targetLine);

    update();
  }

  void update() {

    math::Vector3 v1 = math::Vector3::fromMatrixPosition( _light->matrixWorld() );
    math::Vector3 v2 = math::Vector3::fromMatrixPosition( _light->target()->matrixWorld() );
    math::Vector3 v3 = v2- v1;

    _lightPlane->lookAt( v3 );

    if ( _color ) {

      _lightPlane->material<0>()->color = _color;
      _targetLine->material<0>()->color = _color;
    }
    else {

      _lightPlane->material<0>()->color = _light->color();
      _targetLine->material<0>()->color = _light->color();
    }

    _targetLine->lookAt( v3 );
    _targetLine->scale().z() = v3.length();
  };

public:
  using Ptr = std::shared_ptr<DirectionalLight>;
  static Ptr make(three::DirectionalLight::Ptr light, float size=1.0f, const Color &color=Color::null())
  {
    return Ptr(new DirectionalLight(light, size, color));
  }
};

}
}

#endif //THREE_PP_DIRECTIONALLIGHT_H
