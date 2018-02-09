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
class Mesh : public virtual Object3D
{
  DrawMode _drawMode;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  Mesh() : Object3D(object::ResolverT<Mesh>::make(*this)), _drawMode(DrawMode::Triangles)
  {}
  Mesh(object::Resolver::Ptr resolver) : Object3D(resolver), _drawMode(DrawMode::Triangles)
  {}

public:
  using Ptr = std::shared_ptr<Mesh>;

  DrawMode drawMode() const {return _drawMode;}

  void setDrawMode(DrawMode mode) {_drawMode = mode;}

  bool renderable() const override {return true;}

  bool frontFaceCW() const override
  {
    return _matrixWorld.determinant() < 0;
  }

  const std::vector<float> &morphTargetInfluences() const {return _morphTargetInfluences;}

  float morphTargetInfluence(unsigned index) const {return _morphTargetInfluences.at(index);}

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;
};

/**
 * mesh template with geometry and material
 *
 * @tparam Geom
 * @tparam Mat
 */
template <typename Geom, typename Mat>
class MeshT : public Mesh, public Object3D_GM<Geom, Mat>
{
  using GeometryPtr = std::shared_ptr<Geom>;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  MeshT(const GeometryPtr &geometry, object::Resolver::Ptr resolver, std::shared_ptr<Mat> material)
     : Object3D(resolver), Object3D_GM<Geom, Mat>(geometry, resolver, material)
  {
    setDrawMode(DrawMode::Triangles);
  }

  MeshT(const GeometryPtr &geometry, std::shared_ptr<Mat> material)
     : Object3D(object::ResolverT<Mesh>::make(*this)), Object3D_GM<Geom, Mat>(geometry, nullptr, material)
  {
    setDrawMode(DrawMode::Triangles);
  }

public:
  using Ptr = std::shared_ptr<MeshT<Geom, Mat>>;

  static Ptr make(const GeometryPtr &geometry, const std::shared_ptr<Mat> &material)
  {
    return Ptr(new MeshT(geometry, material));
  }

  static Ptr make(std::string name, const GeometryPtr &geometry, const std::shared_ptr<Mat> &material)
  {
    Ptr p(new MeshT(geometry, material));
    p->_name = name;
    return p;
  }
};

}


#endif //THREEPP_MESH
