//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_MESHDISTANCEMATERIAL_H
#define THREE_QT_MESHDISTANCEMATERIAL_H

#include "Material.h"
#include <math/Vector3.h>
#include <textures/Texture.h>

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
class MeshDistanceMaterial : public MaterialT<
   material::DisplacementMap,
   material::AlphaMap>
{
  MeshDistanceMaterial(bool morphing, bool skinning)
     : MaterialT(material::ResolverT<MeshDistanceMaterial>::make(*this))
  {
    this->morphTargets = morphing;
    this->morphNormals = morphing;
    this->skinning = skinning;
  }

public:
  math::Vector3 referencePosition;
  float nearDistance = 1;
  float farDistance = 1000;

  using Ptr = std::shared_ptr<MeshDistanceMaterial>;
  static Ptr make(bool morphing, bool skinning) {return Ptr(new MeshDistanceMaterial(morphing, skinning));}

  void setupPointLight(const math::Vector3 &position, float near, float far) override;
};

}
#endif //THREE_QT_MESHDISTANCEMATERIAL_H
