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

template <typename Geom, typename Mat>
class Mesh_T : public Mesh, public Object3D_GM<Geom, Mat>
{
  using GeometryPtr = std::shared_ptr<Geom>;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  Mesh_T(const GeometryPtr &geometry, std::shared_ptr<Mat> material)
     : Object3D_GM<Geom, Mat>(geometry, nullptr, material)
  {
    setDrawMode(DrawMode::Triangles);
  }

  Mesh_T() : Object3D_GM<Geom, Mat>(BufferGeometry::make(), nullptr, Mat::make())
  {
    setDrawMode(DrawMode::Triangles);
  }

public:
  using Ptr = std::shared_ptr<Mesh_T>;
  static Ptr make() {return std::shared_ptr<Mesh_T>(new Mesh_T());}
  static Ptr make(const GeometryPtr &geometry, const std::shared_ptr<Mat> &material)
  {
    return Ptr(new Mesh_T(geometry, material));
  }
};

}


#endif //THREEQT_MESH
