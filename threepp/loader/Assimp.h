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

/**
 * Assimp-based scene Loader
 */
class Assimp : public Loader, private AssimpOptions
{
protected:
  std::vector<Material::Ptr> _materials;
  std::vector<Mesh::Ptr> _meshes;

  std::shared_ptr<::Assimp::Importer> _importer;

  void loadScene(std::string name, ResourceLoader &loader);

public:
  Assimp(const AssimpOptions &options = AssimpOptions()) : AssimpOptions(options) {}
  ~Assimp() override;

  void substituteShading(ShadingModel old, ShadingModel subst)
  {
    modelMap[old] = subst;
  }

  void load(std::string name, ResourceLoader &loader) override;
  void load(std::string name, Color background, ResourceLoader &loader) override;
  void load(std::string name, Texture::Ptr background, ResourceLoader &loader) override;
};

}
}

#endif //THREEPP_ASSIMP_H
