//
// Created by byter on 1/7/18.
//

#ifndef THREEPP_OBJECTS_H
#define THREEPP_OBJECTS_H

#include <core/Object3D.h>

namespace three {

class Objects : public Object3D
{
protected:
  Objects() : Object3D(object::ResolverT<Objects>::make(*this)) {}

public:
  using Ptr = std::shared_ptr<Objects>;
  static Ptr make() {
    return Ptr(new Objects());
  }
};

}

#endif //THREEPP_OBJECTS_H
