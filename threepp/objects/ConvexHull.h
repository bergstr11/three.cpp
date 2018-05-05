//
// Created by byter on 5/1/18.
//

#ifndef THREE_PP_CONVEXHULL_H
#define THREE_PP_CONVEXHULL_H

#include <vector>
#include <threepp/core/Object3D.h>

namespace three {

class ConvexHull : public Object3D
{
  std::vector<Object3D::Ptr> _objects;

  explicit ConvexHull(const std::vector<Object3D::Ptr> &objects);

public:
  using Ptr = std::shared_ptr<ConvexHull>;
  Ptr make(const std::vector<Object3D::Ptr> &objects) {
    return Ptr(new ConvexHull(objects));
  }

  ConvexHull *cloned() const override
  {
    return new ConvexHull(*this);
  }

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) override;
};

}


#endif //THREE_PP_CONVEXHULL_H
