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

  //map = null;

  //lightMap = null;
  float lightMapIntensity = 1.0;

  //aoMap = null;
  float aoMapIntensity = 1.0;

  //specularMap = null;

  //alphaMap = null;

  //envMap = null;
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
  : Material(morphTargets, false, skinning)
  {

  }

public:
  using Ptr = std::shared_ptr<MeshBasicMaterial>;
  static Ptr make(bool morphTargets, bool skinning) {
    return std::shared_ptr<MeshBasicMaterial>(new MeshBasicMaterial(morphTargets, skinning));
  }


};

}


#endif //THREEQT_MESHBASICMATERIAL
