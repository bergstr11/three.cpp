//
// Created by byter on 27.09.17.
//

#ifndef THREE_QT_POINTS_H
#define THREE_QT_POINTS_H

#include <three/core/Object3D.h>

namespace three {

class Points : public Object3D
{
  Points() : Object3D(object::ResolverT<Points>::make(*this)) {}
public:
  using Ptr = std::shared_ptr<Points>;

  bool renderable() const override {return true;}
};

}
#endif //THREE_QT_POINTS_H
