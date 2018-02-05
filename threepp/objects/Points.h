//
// Created by byter on 27.09.17.
//

#ifndef THREEPP_POINTS_H
#define THREEPP_POINTS_H

#include <threepp/core/Object3D.h>

namespace three {

class Points : public Object3D
{
  Points() : Object3D(object::ResolverT<Points>::make(*this)) {}
public:
  using Ptr = std::shared_ptr<Points>;

  bool renderable() const override {return true;}
};

}
#endif //THREEPP_POINTS_H
