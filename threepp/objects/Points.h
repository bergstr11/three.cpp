//
// Created by byter on 27.09.17.
//

#ifndef THREEPP_POINTS_H
#define THREEPP_POINTS_H

#include <threepp/core/Object3D.h>
#include <threepp/material/PointsMaterial.h>

namespace three {

class Points : public Object3D_G<BufferGeometry>
{
protected:
  Points(const BufferGeometry::Ptr geometry, const Material::Ptr material)
     : Object3D_G(geometry, object::ResolverT<Points>::make(*this)),
       Object3D(object::ResolverT<Points>::make(*this)), _material(material)
  {}

  Material::Ptr _material;

public:
  using Ptr = std::shared_ptr<Points>;

  static Ptr make(const BufferGeometry::Ptr geometry=BufferGeometry::make(),
                  const Material::Ptr material=PointsMaterial::make(Color(0xffffff * 1.0f / rand())))
  {
    return Ptr(new Points(geometry, material));
  }

  bool renderable() const override {return true;}

  const Material::Ptr material() const override {return _material;}

  const Material::Ptr material(size_t index) const override
  {
    if(index != 0) throw std::out_of_range("material index must be 0");
    return _material;
  }

  const size_t materialCount() const override {return _material ? 1 : 0;}

  const Geometry::Ptr geometry() const override {return _geometry;}

  const BufferGeometry::Ptr buffergeometry() const {return _geometry;}
};

}
#endif //THREEPP_POINTS_H
