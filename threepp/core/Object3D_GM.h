//
// Created by byter on 6/24/18.
//

#ifndef THREE_PP_OBJECT3D_GM_H
#define THREE_PP_OBJECT3D_GM_H

#include "Object3D.h"

namespace three {

/**
 * mixin type that provides typed access to geometry and materials
 *
 * @tparam Geom
 * @tparam Mat
 */
template <typename Geom=BufferGeometry, typename ... Mat>
class Object3D_GM
{
  //typed, synchronized copy of owner._materials
  std::tuple<std::shared_ptr<Mat> ...> _materialsTuple;
  Object3D &_owner;

protected:
  using GeometryPtr = std::shared_ptr<Geom>;

  //typed, synchronized copy of owner._geometry
  GeometryPtr _geometry;

  explicit Object3D_GM(Object3D &owner, const Object3D_GM &other)
     : _owner(owner), _geometry(other._geometry), _materialsTuple(other._materialsTuple)
  {}

  explicit Object3D_GM(Object3D &owner, GeometryPtr geometry, std::tuple<std::shared_ptr<Mat> ...> mats)
  : _materialsTuple(mats), _owner(owner)
  {
  }

  explicit Object3D_GM(Object3D &owner, GeometryPtr geometry, std::shared_ptr<Mat> ... args)
     : _materialsTuple(args...), _geometry(geometry), _owner(owner)
  {
  }

public:
  GeometryPtr geometry_t() const {return _geometry;}

  template<int N>
  decltype(std::get<N>(_materialsTuple)) material_t()
  {
    return std::get<N>(_materialsTuple);
  }

  template<int N>
  typename std::remove_reference<decltype(std::get<N>(_materialsTuple))>::type material_t() const
  {
    return std::get<N>(_materialsTuple);
  }

  template<int N, typename T=decltype(std::get<N>(_materialsTuple))>
  void setMaterial(T material) {
    std::get<N>(_materialsTuple) = material;
    _owner._materials.resize(N);
    _owner._materials[N] = material;
  }
};

}
#endif //THREE_PP_OBJECT3D_GM_H
