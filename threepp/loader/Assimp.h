//
// Created by byter on 1/6/18.
//

#ifndef THREEPP_ASSIMP_H
#define THREEPP_ASSIMP_H

#include <vector>
#include <istream>
#include <string>
#include <unordered_map>

#include <threepp/material/Material.h>
#include <threepp/objects/Mesh.h>
#include <threepp/scene/Scene.h>
#include <threepp/util/simplesignal.h>

#include "Loader.h"

namespace Assimp {
class Importer;
}

namespace three {
namespace loader {

enum class ShadingModel {Phong, Toon, Gouraud, Flat};

struct AssimpOptions
{
  enum_map<ShadingModel, ShadingModel> modelMap;

  AssimpOptions() {
    modelMap[ShadingModel::Phong] = ShadingModel::Phong;
    modelMap[ShadingModel::Gouraud] = ShadingModel::Phong;
    modelMap[ShadingModel::Toon] = ShadingModel::Toon;
    modelMap[ShadingModel::Flat] = ShadingModel::Flat;
  }
};

using AssimpMaterialHandler = loader::MaterialHandler<
   MeshPhongMaterial, MeshToonMaterial, MeshLambertMaterial, MeshPhysicalMaterial, MeshStandardMaterial, MeshBasicMaterial>;

/**
 * Assimp-based scene Loader
 */
class DLX Assimp : public Loader, private AssimpOptions
{
public:
  Assimp(const AssimpMaterialHandler *materialHandler, const AssimpOptions &options=AssimpOptions())
     : AssimpOptions(options), _materialHandler(materialHandler)
  {}

  Assimp(const AssimpOptions &options=AssimpOptions()) : AssimpOptions(options)
  {}

  ~Assimp() override;

  void substituteShading(ShadingModel old, ShadingModel subst)
  {
    modelMap[old] = subst;
  }

  void load(std::string name, ResourceLoader &loader) override;
  void load(std::string name, const Color &background, ResourceLoader &loader) override;
  void load(std::string name, Texture::Ptr background, ResourceLoader &loader) override;

protected:
  std::vector<Material::Ptr> _materials;

  std::shared_ptr<::Assimp::Importer> _importer;

  const AssimpMaterialHandler *_materialHandler = nullptr;

  void loadScene(std::string name, ResourceLoader &loader);
};

}
}

#endif //THREEPP_ASSIMP_H
