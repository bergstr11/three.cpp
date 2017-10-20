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
     : Object3D(geometry), _drawMode(DrawMode::Triangles)
  {}

  Mesh() : Object3D(BufferGeometry::make()), _drawMode(DrawMode::Triangles)
  {}

public:
  using Ptr = std::shared_ptr<Mesh>;

  Signal<void()> onBeforeRender;

  bool renderable() const override {return true;}

  const std::vector<float> morphTargetInfluences() const {return _morphTargetInfluences;}

  float morphTargetInfluence(unsigned index) const {return _morphTargetInfluences.at(index);}

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;
};

template <typename Mat>
class MeshBase : public Mesh, public Object3DBase<Mat>
{
  DrawMode _drawMode;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  MeshBase(const Geometry::Ptr &geometry, std::shared_ptr<Mat> material)
     : Object3DBase<Mat>(geometry, material), _drawMode(DrawMode::Triangles)
  {
  }

  MeshBase() : Object3DBase<Mat>(BufferGeometry::make(), Mat::make()), _drawMode(DrawMode::Triangles)
  {
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
