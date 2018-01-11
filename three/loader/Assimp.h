//
// Created by byter on 1/6/18.
//

#ifndef THREEPP_ASSIMP_H
#define THREEPP_ASSIMP_H

#include <vector>
#include <istream>
#include <string>

#include <material/Material.h>
#include <objects/Mesh.h>
#include <scene/Scene.h>
#include <helper/simplesignal.h>

#include "Loader.h"

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

  void loadScene(std::string name, ResourceLoader &loader);

public:
  void load(std::string name, ResourceLoader &loader) override;
  void load(std::string name, Color background, ResourceLoader &loader) override;
  void load(std::string name, Texture::Ptr background, ResourceLoader &loader) override;
};

}
}

#endif //THREEPP_ASSIMP_H
