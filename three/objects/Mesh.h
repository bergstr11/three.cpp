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

template <typename Mat=MeshBasicMaterial>
class Mesh : public Object3DBase<Mat>
{
  DrawMode _drawMode;

  std::vector<float> _morphTargetInfluences;
  std::unordered_map<std::string, MorphTarget> _morphTargetDictionary;

protected:
  Mesh(const Geometry::Ptr &geometry, std::shared_ptr<Mat> material)
     : Object3DBase<Mat>(geometry, material), _drawMode(DrawMode::Triangles)
  {
  }

  Mesh() : Object3DBase(BufferGeometry::make(), MeshBasicMaterial::make()), _drawMode(DrawMode::Triangles)
  {
  }

public:
  using Ptr = std::shared_ptr<Mesh>;
  static Ptr make() {return std::shared_ptr<Mesh>(new Mesh());}
  static Ptr make(const Geometry::Ptr &geometry, const Material::Ptr &material)
  {
    return std::shared_ptr<Mesh>(new Mesh(geometry, material));
  }

  Signal<void()> onBeforeRender;

  bool renderable() const override {return true;}

  const std::vector<float> morphTargetInfluences() const {return _morphTargetInfluences;}

  float morphTargetInfluence(unsigned index) const {return _morphTargetInfluences.at(index);}

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;
};

}


#endif //THREEQT_MESH
