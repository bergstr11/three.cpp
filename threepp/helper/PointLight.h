//
// Created by byter on 2/16/18.
//

#ifndef THREE_PP_POINTLIGHT_H
#define THREE_PP_POINTLIGHT_H

#include <threepp/objects/Mesh.h>
#include <threepp/light/PointLight.h>
#include <threepp/geometry/Sphere.h>
#include <threepp/material/MeshBasicMaterial.h>

namespace three {
namespace helper {

class PointLight : public Mesh
{
  three::PointLight::Ptr _light;
  Color _color;

protected:
  PointLight(three::PointLight::Ptr light, float sphereSize, const Color &color)
     : Mesh(geometry::buffer::Sphere::make(sphereSize, 4, 2), {MeshBasicMaterial::make()}),
       _light(light), _color(color)
  {
    material()->wireframe = true;
    matrixAutoUpdate = false;

    update();
  }

  void update()
  {
    MeshBasicMaterial *mat = material(0)->typer;
    if (_color) {
      mat->color = _color;
    }
    else {
      mat->color = _light->color();
    }
  }

public:
  Object3D *cloned() const override
  {
    //not me!
    return nullptr;
  }
};

}
}

#endif //THREE_PP_POINTLIGHT_H
