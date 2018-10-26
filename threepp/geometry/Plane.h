//
// Created by byter on 20.08.17.
//

#ifndef THREEPP_PLANE_H
#define THREEPP_PLANE_H

#include <cstdint>
#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

struct PlaneParams
{
  float width;
  float height;
  float widthSegments;
  float heightSegments;

  PlaneParams(float width, float height, float widthSegments, float heightSegments)
     : width(width), height(height), widthSegments(widthSegments),
       heightSegments(heightSegments)
  {}
};

class Plane : public LinearGeometry, public PlaneParams
{
  Plane(const Plane &plane) : LinearGeometry(plane), PlaneParams(plane) {}

protected:
  Plane(float width, float height, float widthSegments, float heightSegments);

public:
  using Ptr = std::shared_ptr<Plane>;

  static Ptr make(float width, float height, float widthSegments, float heightSegments)
  {
    return Ptr(new Plane(width, height, widthSegments, heightSegments));
  }

  Plane *cloned() const override
  {
    return new Plane(*this);
  }
};

namespace buffer {

class Plane : public BufferGeometry, public PlaneParams
{
  Plane(const Plane &plane) : BufferGeometry(plane), PlaneParams(plane) {}

public:
  Plane(float width, float height, float widthSegments = 1, float heightSegments = 1);

  using Ptr = std::shared_ptr<Plane>;

  static Ptr make(float width, float height, float widthSegments = 1, float heightSegments = 1)
  {
    return Ptr(new Plane(width, height, widthSegments, heightSegments));
  }

  Plane *cloned() const override
  {
    return new Plane(*this);
  }
};

}

}
}

#endif //THREEPP_PLANE_H
