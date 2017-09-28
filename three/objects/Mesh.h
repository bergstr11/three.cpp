//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_MESH
#define THREEQT_MESH

#include <core/Object3D.h>
#include <material/Material.h>
#include <material/MeshBasicMaterial.h>

namespace three {

class Mesh : public Object3D
{
  DrawMode _drawMode;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  Mesh(const Geometry::Ptr &geometry, const Material::Ptr &material)
     : Object3D(geometry), _drawMode(DrawMode::Triangles)
  {
    _materials.push_back(material);
  }

  Mesh()
     : Object3D(BufferGeometry::make()), _drawMode(DrawMode::Triangles)
  {
    _materials.push_back(MeshBasicMaterial::make());
  }

public:
  using Ptr = std::shared_ptr<Mesh>;
  static Ptr make() {return std::shared_ptr<Mesh>(new Mesh());}
  static Ptr make(const Geometry::Ptr &geometry, const Material::Ptr &material)
  {
    return std::shared_ptr<Mesh>(new Mesh(geometry, material));
  }

  bool renderable() const override {return true;}

  float morphTargetInfluence(unsigned index) const {return _morphTargetInfluences.at(index);}

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;
};

}


#endif //THREEQT_MESH
