//
// Created by byter on 5/1/18.
//

#ifndef THREE_PP_CONVEXHULL_H
#define THREE_PP_CONVEXHULL_H

#include <vector>
#include <threepp/core/Object3D.h>

struct qh_mesh_t;

namespace three {

class ConvexHull : public Object3D
{
  const Object3D::Ptr _object;
  qh_mesh_t *mesh = nullptr;

  explicit ConvexHull(const Object3D::Ptr &object);

public:
  ~ConvexHull();

  using Ptr = std::shared_ptr<ConvexHull>;
  Ptr make(const Object3D::Ptr &object) {
    return Ptr(new ConvexHull(object));
  }

  ConvexHull *cloned() const override
  {
    return new ConvexHull(*this);
  }

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) override;
};

}


#endif //THREE_PP_CONVEXHULL_H
