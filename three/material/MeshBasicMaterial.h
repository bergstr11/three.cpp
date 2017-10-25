//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_MESHBASICMATERIAL
#define THREEQT_MESHBASICMATERIAL

#include "Material.h"
#include "core/Color.h"

namespace three {

struct MeshBasicMaterial : public Material
{
  Color color {0xffffff}; // emissive

  Texture::Ptr map;

  //lightMap = null;
  float lightMapIntensity = 1.0;

  //aoMap = null;
  float aoMapIntensity = 1.0;

  CombineOperation combine = CombineOperation::Multiply;
  float reflectivity = 1;
  float refractionRatio = 0.98;

  bool wireframe = false;
  float wireframeLinewidth = 1;
  LineCap wireframeLinecap = LineCap::round;
  LineJoin wireframeLinejoin = LineJoin::round;

  bool skinning = false;
  bool morphTargets = false;

  bool lights = false;

private:
  MeshBasicMaterial(bool morphTargets, bool skinning)
  {
    this->morphTargets = morphTargets;
    this->skinning = skinning;
  }

  MeshBasicMaterial() : Material(material::Resolver<MeshBasicMaterial>::make(*this))
  {}

public:
  using Ptr = std::shared_ptr<MeshBasicMaterial>;

  static Ptr make(bool morphTargets, bool skinning) {
    return std::shared_ptr<MeshBasicMaterial>(new MeshBasicMaterial(morphTargets, skinning));
  }

  static Ptr make() {
    return std::shared_ptr<MeshBasicMaterial>(new MeshBasicMaterial());
  }
};

}


#endif //THREEQT_MESHBASICMATERIAL
