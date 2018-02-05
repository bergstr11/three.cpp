//
// Created by byter on 1/6/18.
//

#ifndef THREEPP_ASSIMP_H
#define THREEPP_ASSIMP_H

#include <vector>
#include <istream>
#include <string>

#include <threepp/material/Material.h>
#include <threepp/objects/Mesh.h>
#include <threepp/scene/Scene.h>
#include <threepp/helper/simplesignal.h>

#include "Loader.h"

namespace Assimp {
class Importer;
}

namespace three {
namespace loader {

/**
 * Assimp-based scene Loader
 */
class Assimp : public Loader
{
protected:
  std::vector<Material::Ptr> _materials;
  std::vector<Mesh::Ptr> _meshes;

  std::shared_ptr<::Assimp::Importer> _importer;

  void loadScene(std::string name, ResourceLoader &loader);

public:
  ~Assimp() override;

  void load(std::string name, ResourceLoader &loader) override;
  void load(std::string name, Color background, ResourceLoader &loader) override;
  void load(std::string name, Texture::Ptr background, ResourceLoader &loader) override;
};

}
}

#endif //THREEPP_ASSIMP_H
