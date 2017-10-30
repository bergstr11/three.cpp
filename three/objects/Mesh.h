//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_MESH
#define THREEQT_MESH

#include <core/Object3D.h>
#include <material/Material.h>
#include <material/MeshBasicMaterial.h>
#include <helper/simplesignal.h>

namespace three {

class Mesh : public virtual Object3D
{
  DrawMode _drawMode;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  Mesh(const Geometry::Ptr &geometry)
     : Object3D(geometry, object::ResolverT<Mesh>::make(*this)), _drawMode(DrawMode::Triangles)
  {}
  Mesh() : Object3D(BufferGeometry::make(), object::ResolverT<Mesh>::make(*this)), _drawMode(DrawMode::Triangles)
  {}

public:
  using Ptr = std::shared_ptr<Mesh>;

  Signal<void()> onBeforeRender;

  DrawMode drawMode() const {return _drawMode;}

  void setDrawMode(DrawMode mode) {_drawMode = mode;}

  bool renderable() const override {return true;}

  const std::vector<float> &morphTargetInfluences() const {return _morphTargetInfluences;}

  float morphTargetInfluence(unsigned index) const {return _morphTargetInfluences.at(index);}

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;
};

template <typename Mat>
class MeshBase : public Mesh, public Object3DMat<Mat>
{
  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  MeshBase(const Geometry::Ptr &geometry, std::shared_ptr<Mat> material)
     : Object3DMat<Mat>(geometry, nullptr, material)
  {
    setDrawMode(DrawMode::Triangles);
  }

  MeshBase() : Object3DMat<Mat>(BufferGeometry::make(), nullptr, Mat::make())
  {
    setDrawMode(DrawMode::Triangles);
  }

public:
  using Ptr = std::shared_ptr<MeshBase>;
  static Ptr make() {return std::shared_ptr<MeshBase>(new MeshBase());}
  static Ptr make(const Geometry::Ptr &geometry, const std::shared_ptr<Mat> &material)
  {
    return Ptr(new MeshBase(geometry, material));
  }
};

}


#endif //THREEQT_MESH
