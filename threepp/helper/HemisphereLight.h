//
// Created by byter on 2/16/18.
//

#ifndef THREE_PP_HEMISPHERELIGHT_H
#define THREE_PP_HEMISPHERELIGHT_H

#include <threepp/light/HemisphereLight.h>
#include <threepp/geometry/Octahedron.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/objects/Mesh.h>

namespace three {
namespace helper {

class HemisphereLight : public Object3D
{
  three::HemisphereLight::Ptr _light;
  Color _color = Color::null();

  geometry::buffer::Octahedron::Ptr _geometry;
  MeshBasicMaterial::Ptr _material;

protected:
  HemisphereLight(const three::HemisphereLight::Ptr &light, float radius, const Color &color)
     : Object3D(), _light(light), _color(color)
  {
    _light->updateMatrixWorld(true);
    _matrix = _light->matrixWorld();
    matrixAutoUpdate = false;

    _geometry = geometry::buffer::Octahedron::make(radius, 0);
    _geometry->rotateY(M_PI * 0.5f);

    _material = MeshBasicMaterial::make();
    _material->wireframe = true;

    if(!_color) _material->vertexColors = Colors::Vertex;

    _geometry->setColor(attribute::prealloc<float, Color>(_geometry->position()->itemCount()));

    add(Mesh::make("hemi_helper", _geometry, _material));
    update();
  }

  void update()
  {
    if (_color) {

      _material->color = _color;
    }
    else {

      for(unsigned i = 0, l = _geometry->color()->itemCount(); i < l; i ++ ) {

        auto &color = _geometry->color()->item_at<Color>(i);
        color = ( i < ( l / 2 ) ) ? _light->skyColor() : _light->groundColor();
      }

      _geometry->color()->needsUpdate();
    }

    math::Vector3 target = math::Vector3::fromMatrixPosition(_light->matrixWorld()).negate();
    _children.front()->lookAt(target);
  }

public:
  using Ptr = std::shared_ptr<HemisphereLight>;
  static Ptr make(const three::HemisphereLight::Ptr &light, float radius, const Color &color) {
    return Ptr(new HemisphereLight(light, radius, color));
  }

  HemisphereLight *cloned() const override
  {
    //not me!
    return nullptr;
  }
};

}
}

#endif //THREE_PP_HEMISPHERELIGHT_H
