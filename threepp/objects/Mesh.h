//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_MESH
#define THREEPP_MESH

#include <threepp/core/Object3D.h>
#include <threepp/material/Material.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/util/simplesignal.h>

namespace three {

/**
 * a mesh without geometry and material
 */
class DLX Mesh : public Object3D
{
  DrawMode _drawMode;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  Mesh() : Object3D(), _drawMode(DrawMode::Triangles)
  {
    Object3D::typer = object::Typer(this);
  }

  Mesh(const Mesh &mesh) : Object3D(mesh)
  {
    _drawMode = mesh._drawMode;
    _morphTargetInfluences = mesh._morphTargetInfluences;
    _morphTargetDictionary = mesh._morphTargetDictionary;
    Object3D::typer = object::Typer(this);
  }

  Mesh(Geometry::Ptr geometry, std::initializer_list<Material::Ptr> materials)
     : Object3D(geometry, materials), _drawMode(DrawMode::Triangles)
  {}

public:
  using Ptr = std::shared_ptr<Mesh>;

  DrawMode drawMode() const {return _drawMode;}

  void setDrawMode(DrawMode mode) {_drawMode = mode;}

  bool isShadowRenderable() const override {return true;}

  bool frontFaceCW() const override
  {
    return _matrixWorld.determinant() < 0;
  }

  const std::vector<float> &morphTargetInfluences() const {return _morphTargetInfluences;}

  float morphTargetInfluence(unsigned index) const {return _morphTargetInfluences.at(index);}

  void raycast(const Raycaster &raycaster, IntersectList &intersects) override;

  Mesh *cloned() const override {
    return object::Typer::set(new Mesh(*this));
  }
};

/**
 * a mesh which can by dynamically extended with materials and geometry
 */
class DLX DynamicMesh : public Mesh
{
protected:
  DynamicMesh() : Mesh()
  {
    Object3D::typer = object::Typer(this);
    typer.allow<Mesh>();
  }

  DynamicMesh(const DynamicMesh &mesh) : Mesh(mesh)
  {
    Object3D::typer = object::Typer(this);
    typer.allow<Mesh>();
  }

  DynamicMesh(Geometry::Ptr geometry, Material::Ptr material) : Mesh(geometry, {})
  {
    Object3D::typer = object::Typer(this);
    typer.allow<Mesh>();

    if(material) setMaterial(material);
  }

public:
  using Ptr = std::shared_ptr<DynamicMesh>;

  static Ptr make(Geometry::Ptr geometry, Material::Ptr material=nullptr)
  {
    return Ptr(new DynamicMesh(geometry, material));
  };

  void setGeometry(Geometry::Ptr geometry) {
    _geometry = geometry;
  }

  void addMaterial(Material::Ptr material) {
    _materials.push_back(material);
  }

  void setMaterial(Material::Ptr material) {
    _materials.resize(1);
    _materials[0] = material;
  }

  template <typename Geom> Geom *geometry_t() {
    return _geometry->typer;
  }
};

}


#endif //THREEPP_MESH
