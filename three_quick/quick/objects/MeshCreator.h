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

  std::shared_ptr<Geometry> geometry;

  Mesh::Ptr mesh;

  MeshCreatorG(const char *name) : name(name) {}

  void set(std::shared_ptr<Geometry> geometry) {
    this->geometry = geometry;
  }
  void material(MeshBasicMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry, MeshBasicMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry, MeshBasicMaterial>>(mesh);
      if(mp) mp->template setMaterial<0> (material);
    }
    else mesh = MeshT<Geometry, MeshBasicMaterial>::make(name, geometry, material);
  }
  void material(MeshLambertMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry, MeshLambertMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry, MeshLambertMaterial>>(mesh);
      if(mp) mp->template setMaterial<0>(material);
    }
    else mesh = MeshT<Geometry, MeshLambertMaterial>::make(name, geometry, material);
  }
  void material(MeshPhongMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry, MeshPhongMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry, MeshPhongMaterial>>(mesh);
      if(mp) mp->template setMaterial<0>(material);
    }
    else mesh = MeshT<Geometry, MeshPhongMaterial>::make(name, geometry, material);
  }
  void material(ShaderMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry, ShaderMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry, ShaderMaterial>>(mesh);
      if(mp) mp->template setMaterial<0>(material);
    }
    else mesh = MeshT<Geometry, ShaderMaterial>::make(name, geometry, material);
  }
};

}
}


#endif //THREEPP_MESHCREATOR_H