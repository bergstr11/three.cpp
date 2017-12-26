//
// Created by byter on 12/22/17.
//

#ifndef THREEPP_SPOTLIGHTHELPER_H
#define THREEPP_SPOTLIGHTHELPER_H

#include <objects/Line.h>
#include <core/BufferGeometry.h>
#include <material/LineMaterial.h>
#include <light/SpotLight.h>

namespace three {
namespace helper {

class SpotLight : public Object3D
{
  three::SpotLight::Ptr _light;
  LineBasicMaterial::Ptr _coneMaterial;
  LineSegments::Ptr _cone;
  Color _color;

protected:
  SpotLight(const three::SpotLight::Ptr &light, const Color &color)
     : Object3D(object::Resolver::makeNull()), _light(light), _color(color)
  {
    _light->updateMatrixWorld(true);
    _matrix = _light->matrixWorld();
    _matrixAutoUpdate = false;

    std::vector<Vertex> positions {
       {0, 0, 0}, {0, 0, 1},
       {0, 0, 0}, {1, 0, 1},
       {0, 0, 0}, {-1, 0, 1},
       {0, 0, 0}, {0, 1, 1},
       {0, 0, 0}, {0, -1, 1}
    };

    for (unsigned i = 0, j = 1, l = 32; i < l; i ++, j ++ ) {

      double p1 = ( (double)i / l ) * M_PI * 2;
      double p2 = ( (double)j / l ) * M_PI * 2;

      positions.emplace_back(std::cos( p1 ), std::sin( p1 ), 1);
      positions.emplace_back(std::cos( p2 ), std::sin( p2 ), 1);
    }

    auto geometry = BufferGeometry::make();
    geometry->setPosition(BufferAttributeT<float>::make(positions));

    _coneMaterial = LineBasicMaterial::make();
    _cone = LineSegments::make("spothelper", geometry, _coneMaterial);
    add(_cone);

    update();
  }

public:
  using Ptr = std::shared_ptr<SpotLight>;
  static Ptr make(const std::string &name, const three::SpotLight::Ptr &light, const Color &color=Color()) {
    Ptr p(new SpotLight(light, color));
    p->_name = name;
    return p;
  }

  void update()
  {
    _light->updateMatrixWorld(true);

    float coneLength = _light->distance() >  0 ? _light->distance() : 1000.0f;
    float coneWidth = coneLength * (float)tan( _light->angle() );

    _cone->scale().set( coneWidth, coneWidth, coneLength );

    math::Vector3 vector = math::Vector3::fromMatrixPosition(_light->matrixWorld());
    math::Vector3 vector2 = math::Vector3::fromMatrixPosition(_light->target()->matrixWorld());

    _cone->lookAt(vector2 - vector );

    if (_color.isNull()) {
      _coneMaterial->color = _light->color();
    }
    else {
      _coneMaterial->color = _color;
    }
  }
};

}
}

#endif //THREEPP_SPOTLIGHTHELPER_H
