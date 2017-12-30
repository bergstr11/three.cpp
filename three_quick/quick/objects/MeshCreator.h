//
// Created by byter on 12/22/17.
//

#ifndef THREEPP_MESHCREATOR_H
#define THREEPP_MESHCREATOR_H

#include <objects/Mesh.h>
#include <material/MeshBasicMaterial.h>
#include <material/MeshLambertMaterial.h>
#include <material/MeshPhongMaterial.h>
#include <material/ShaderMaterial.h>

namespace three {
namespace quick {

struct MeshCreator
{
  virtual void material(MeshBasicMaterial::Ptr material) = 0;
  virtual void material(MeshLambertMaterial::Ptr material) = 0;
  virtual void material(MeshPhongMaterial::Ptr material) = 0;
  virtual void material(ShaderMaterial::Ptr material) = 0;
};

template <typename Geometry>
struct MeshCreatorG : public MeshCreator
{
  const char * const name;
  const std::shared_ptr<Geometry> geometry;

  Mesh::Ptr mesh;

  MeshCreatorG(const char *name, const std::shared_ptr<Geometry> geometry)
     : name(name), geometry(geometry) {}

  void material(MeshBasicMaterial::Ptr material) override {
    mesh = MeshT<Geometry, MeshBasicMaterial>::make(name, geometry, material);
  }
  void material(MeshLambertMaterial::Ptr material) override {
    mesh = MeshT<Geometry, MeshLambertMaterial>::make(name, geometry, material);
  }
  void material(MeshPhongMaterial::Ptr material) override {
    mesh = MeshT<Geometry, MeshPhongMaterial>::make(name, geometry, material);
  }
  void material(ShaderMaterial::Ptr material) override {
    mesh = MeshT<Geometry, ShaderMaterial>::make(name, geometry, material);
  }
};

}
}


#endif //THREEPP_MESHCREATOR_H
