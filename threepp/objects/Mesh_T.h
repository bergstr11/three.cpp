//
// Created by byter on 6/24/18.
//

#ifndef THREE_PP_MESH_T_H
#define THREE_PP_MESH_T_H

#include "Mesh.h"
#include <threepp/core/Object3D_GM.h>

namespace three {

/**
 * mesh template with statically typed geometry and materials
 *
 * @tparam Geom
 * @tparam Mat
 */
template <typename Geom, typename... Mat>
class Mesh_T : public Mesh, public Object3D_GM<Geom, Mat...>
{
  friend class Mesh;

protected:
  Mesh_T(const typename Geom::Ptr &geometry, typename Mat::Ptr... material)
     : Mesh(geometry, {material...}), Object3D_GM<Geom, Mat...>(*this, geometry, material...)
  {
    setDrawMode(DrawMode::Triangles);
  }

  Mesh_T() : Mesh(), Object3D_GM<Geom, Mat...>()
  {
    setDrawMode(DrawMode::Triangles);
  }

  Mesh_T(const Mesh_T &mesh) : Mesh(mesh), Object3D_GM<Geom, Mat...>(*this, mesh)
  {
    setDrawMode(DrawMode::Triangles);
  }

public:
  static Ptr make(const typename Geom::Ptr &geometry, const std::shared_ptr<Mat> &... material)
  {
    return Ptr(new Mesh_T(geometry, material...));
  }

  static Ptr make(const typename Geom::Ptr &geometry, std::tuple<std::shared_ptr<Mat> ...> mats)
  {
    return Ptr(new Mesh_T(geometry, mats));
  }

  static Ptr make(std::string name, const typename Geom::Ptr &geometry, const std::shared_ptr<Mat> &... material)
  {
    Ptr p(new Mesh_T(geometry, material...));
    p->_name = name;
    return p;
  }

  Mesh_T *cloned() const override {
    return new Mesh_T(*this);
  }
};

}
#endif //THREE_PP_MESH_T_H
