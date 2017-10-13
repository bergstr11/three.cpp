//
// Created by byter on 13.10.17.
//

#ifndef THREE_QT_SKELETON_H
#define THREE_QT_SKELETON_H

#include <core/Object3D.h>

namespace three {

class Bone : public Object3D
{
  Bone(Geometry::Ptr geometry) : Object3D(geometry) {}
public:
  using Ptr = std::shared_ptr<Bone>;
  static Ptr make(Geometry::Ptr geometry) {
    return Ptr(new Bone(geometry));
  }
};

class Skeleton
{

};

}
#endif //THREE_QT_SKELETON_H
