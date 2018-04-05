//
// Created by byter on 27.09.17.
//

#ifndef THREEPP_POINTS_H
#define THREEPP_POINTS_H

#include <threepp/core/Object3D.h>
#include <threepp/material/PointsMaterial.h>

namespace three {

class Points : public Object3D_GM<BufferGeometry, PointsMaterial>
{
protected:
  Points(const BufferGeometry::Ptr &geometry, const PointsMaterial::Ptr &material)
     : Object3D(object::ResolverT<Points>::make(*this)),
       Object3D_GM(geometry, nullptr, material)
  {}

  Points(const Points &points)
     : Object3D(points, object::ResolverT<Points>::make(*this)),
       Object3D_GM(BufferGeometry::Ptr(geometry_t()->cloned()), nullptr, PointsMaterial::Ptr(points.material<0>()->cloned()))
  {}

public:
  using Ptr = std::shared_ptr<Points>;

  static Ptr make(const BufferGeometry::Ptr &geometry=BufferGeometry::make(),
                  const PointsMaterial::Ptr &material=PointsMaterial::make(Color(0xffffff * 1.0f / rand())))
  {
    return Ptr(new Points(geometry, material));
  }

  bool isShadowRenderable() const override {return true;}

  Points *cloned() const override
  {
    return new Points(*this);
  }
};

}
#endif //THREEPP_POINTS_H
