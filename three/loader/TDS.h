//
// Created by byter on 1/5/18.
//

#ifndef THREEPP_TDSLOADER_H
#define THREEPP_TDSLOADER_H

#include <vector>
#include <istream>
#include <string>

#include <material/Material.h>
#include <objects/Mesh.h>
#include <objects/Objects.h>
#include <helper/simplesignal.h>

#include "Loader.h"

namespace three {
namespace loader {

struct Chunk;

/*
 * Autodesk 3DS threee.js file loader, based on lib3ds.
 *
 * Loads geometry with uv and materials basic properties with texture support.
 *
 * @author @tentone
 * @author @timknip
 * @class TDSLoader
 */
class TDS : public Loader
{
public:
  void load(std::string name, ResourceLoader &loader) override;

protected:
  bool _useBufferGeometry = false;

  std::vector<Material::Ptr> _materials;
  std::vector<Mesh::Ptr> _meshes;

  size_t _position;

  void readMeshData(std::istream &in, ResourceLoader &loader, Chunk &chunk);
  void readNamedObject(std::istream &in, ResourceLoader &loader, Chunk &chunk);
  void readMaterialEntry(std::istream &in, ResourceLoader &loader, Chunk &chunk);
  Mesh::Ptr readMesh(std::istream &in, std::string name, Chunk &chunk);
};

}
}


#endif //THREEPP_TDSLOADER_H
