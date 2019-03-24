//
// Created by byter on 13.09.17.
//

#ifndef THREEPP_MESHDISTANCEMATERIAL_H
#define THREEPP_MESHDISTANCEMATERIAL_H

#include "Material.h"
#include <threepp/math/Vector3.h>
#include <threepp/textures/Texture.h>

namespace three {

/**
 * @author WestLangley / http://github.com/WestLangley
 *
 * parameters = {
 *
 *  referencePosition: <float>,
 *  nearDistance: <float>,
 *  farDistance: <float>,
 *
 *  skinning: <bool>,
 *  morphTargets: <bool>,
 *
 *  map: new THREE.Texture( <Image> ),
 *
 *  alphaMap: new THREE.Texture( <Image> ),
 *
 *  displacementMap: new THREE.Texture( <Image> ),
 *  displacementScale: <float>,
 *  displacementBias: <float>
 *
 * }
 */
class DLX MeshDistanceMaterial : public MaterialT<
   material::Diffuse,
   material::AlphaMap,
   material::DisplacementMap>
{
  MeshDistanceMaterial()
     : MaterialT(material::InfoT<MeshDistanceMaterial>(), material::Typer(this))
  {}

  MeshDistanceMaterial(const MeshDistanceMaterial &material)
     : MaterialT(material, material::InfoT<MeshDistanceMaterial>(), material::Typer(this))
  {}

protected:
  void callback(const material::Selector &selector) override;

public:
  math::Vector3 referencePosition;
  float nearDistance = 1;
  float farDistance = 1000;

  using Ptr = std::shared_ptr<MeshDistanceMaterial>;
  static Ptr make(bool morphing, bool skinning) {
    Ptr p(new MeshDistanceMaterial());
    p->morphTargets = morphing;
    p->morphNormals = morphing;
    p->skinning = skinning;
    return p;
  }

  MeshDistanceMaterial *cloned() const override {
    return new MeshDistanceMaterial(*this);
  }
};

}
#endif //THREEPP_MESHDISTANCEMATERIAL_H
