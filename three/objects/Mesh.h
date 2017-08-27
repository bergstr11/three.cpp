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
  Geometry::Ptr _geometry;
  Material::Ptr _material;

  DrawMode _drawMode;

protected:
  Mesh(const Geometry::Ptr &geometry, const Material::Ptr &material)
     : _geometry(geometry), _material(material), _drawMode(DrawMode::Triangles) {}

  Mesh()
     : _geometry(BufferGeometry::make()),
       _material(MeshBasicMaterial::make()),
       _drawMode(DrawMode::Triangles) {}

public:
  using Ptr = std::shared_ptr<Mesh>;
  static Ptr make() {return std::shared_ptr<Mesh>(new Mesh());}
  static Ptr make(const Geometry::Ptr &geometry, const Material::Ptr &material)
  {
    return std::shared_ptr<Mesh>(new Mesh(geometry, material));
  }
};

}


#endif //THREEQT_MESH
