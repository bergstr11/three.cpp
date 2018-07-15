//
// Created by byter on 27.09.17.
//

#ifndef THREEPP_POINTS_H
#define THREEPP_POINTS_H

#include <threepp/core/Object3D.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/material/PointsMaterial.h>

namespace three {

class Points : public Object3D
{
protected:
  Points(const BufferGeometry::Ptr &geometry, const PointsMaterial::Ptr &material)
     : Object3D(geometry, material)
  {
    Object3D::typer = object::Typer(this);
  }

  Points(const Points &points) : Object3D(points)
  {
    Object3D::typer = object::Typer(this);
  }

public:
  using Ptr = std::shared_ptr<Points>;

  static Ptr make(const BufferGeometry::Ptr &geometry=BufferGeometry::make(),
                  const PointsMaterial::Ptr &material=PointsMaterial::make(Color(0xffffff * 1.0f / rand())))
  {
    return Ptr(new Points(geometry, material));
  }

  BufferGeometry *bufferGeometry() const {
    return (BufferGeometry *) _geometry->typer;
  }

  PointsMaterial *pointsMaterial() const {
    return (PointsMaterial *)_materials[0]->typer;
  }

  bool isShadowRenderable() const override {return true;}

  Points *cloned() const override
  {
    return new Points(*this);
  }
};

}
#endif //THREEPP_POINTS_H
