//
// Created by byter on 11/4/17.
//

#ifndef THREEPP_MESHPHYSICALMATERIAL_H
#define THREEPP_MESHPHYSICALMATERIAL_H

#include "MeshStandardMaterial.h"

namespace three {

/**
 * @author WestLangley / http://github.com/WestLangley
 *
 * parameters = {
 *  reflectivity: <float>
 * }
 */
struct DLX MeshPhysicalMaterial: public MeshStandardMaterial
{
  std::unordered_map<std::string, std::string> defines {{ "PHYSICAL", "" }};

  float reflectivity = 0.5; // maps to F0 = 0.04

  float clearCoat = 0.0;
  float clearCoatRoughness = 0.0;

protected:
  MeshPhysicalMaterial()
     : MeshStandardMaterial(material::InfoT<MeshPhysicalMaterial>(), material::Typer(this))
  {}

public:
  using Ptr = std::shared_ptr<MeshPhysicalMaterial>;
  static Ptr make() {
    return Ptr(new MeshPhysicalMaterial());
  }
};

}

#endif //THREEPP_MESHPHYSICALMATERIAL_H
