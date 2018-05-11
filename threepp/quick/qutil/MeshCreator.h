//
// Created by byter on 12/22/17.
//

#ifndef THREEPPQ_MESHCREATOR_H
#define THREEPPQ_MESHCREATOR_H

#include <threepp/objects/Mesh.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/material/MeshPhongMaterial.h>
#include <threepp/material/ShaderMaterial.h>

namespace three {
namespace quick {

struct MeshCreator
{
  virtual void material(MeshBasicMaterial::Ptr material) = 0;
  virtual void material(MeshLambertMaterial::Ptr material) = 0;
  virtual void material(MeshPhongMaterial::Ptr material) = 0;
  virtual void material(ShaderMaterial::Ptr material) = 0;

  virtual Mesh::Ptr getMesh() = 0;

  using Ptr = std::shared_ptr<MeshCreator>;
};

template <typename Geometry_t>
struct MeshCreatorG : public MeshCreator
{
  const char * const name;

  std::shared_ptr<Geometry_t> geometry;

  Mesh::Ptr mesh;

  void set(std::shared_ptr<Geometry_t> geometry) {
    this->geometry = geometry;
    this->mesh = nullptr;
  }
  void material(MeshBasicMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry_t, MeshBasicMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry_t, MeshBasicMaterial>>(mesh);
      if(mp) mp->template setMaterial<0> (material);
    }
    else mesh = MeshT<Geometry_t, MeshBasicMaterial>::make(name, geometry, material);
  }
  void material(MeshLambertMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry_t, MeshLambertMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry_t, MeshLambertMaterial>>(mesh);
      if(mp) mp->template setMaterial<0>(material);
    }
    else mesh = MeshT<Geometry_t, MeshLambertMaterial>::make(name, geometry, material);
  }
  void material(MeshPhongMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry_t, MeshPhongMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry_t, MeshPhongMaterial>>(mesh);
      if(mp) mp->template setMaterial<0>(material);
    }
    else mesh = MeshT<Geometry_t, MeshPhongMaterial>::make(name, geometry, material);
  }
  void material(ShaderMaterial::Ptr material) override {
    if(mesh) {
      typename MeshT<Geometry_t, ShaderMaterial>::Ptr mp =
         std::dynamic_pointer_cast<MeshT<Geometry_t, ShaderMaterial>>(mesh);
      if(mp) mp->template setMaterial<0>(material);
    }
    else mesh = MeshT<Geometry_t, ShaderMaterial>::make(name, geometry, material);
  }

  Mesh::Ptr getMesh() override
  {
    return mesh;
  }

  explicit MeshCreatorG(const char *name) : name(name) {}

  using Ptr = std::shared_ptr<MeshCreatorG>;

  static Ptr make(const char *name) {
    return Ptr(new MeshCreatorG(name));
  }
};

}
}


#endif //THREEPPQ_MESHCREATOR_H
