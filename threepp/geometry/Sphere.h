//
// Created by byter on 11/25/17.
//

#ifndef THREEPP_GEOM_SPHERE_H
#define THREEPP_GEOM_SPHERE_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

class Sphere : public LinearGeometry
{
  const unsigned _widthSegments, _heightSegments;
  const float _radius, _phiStart, _phiLength, _thetaStart, _thetaLength;

protected:
  Sphere(float radius, unsigned widthSegments, unsigned heightSegments,
         float phiStart, float phiLength, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Sphere>;
  static Ptr make(float radius=50,
                  unsigned widthSegments=8,
                  unsigned heightSegments=6,
                  float phiStart=0,
                  float phiLength=(float)M_PI * 2,
                  float thetaStart= 0,
                  float thetaLength=(float)M_PI)
  {
    return Ptr(new Sphere(radius,
                          std::max(widthSegments, 3u),
                          std::max(heightSegments, 2u),
                          phiStart, phiLength, thetaStart, thetaLength));
  }

  Sphere *cloned() const override {
    return new Sphere(*this);
  }
};

namespace buffer {

class Sphere : public BufferGeometry
{
friend class three::geometry::Sphere;

  const unsigned  _widthSegments, _heightSegments;
  const float _radius, _phiStart, _phiLength, _thetaStart, _thetaLength;

protected:
  Sphere(float radius, unsigned widthSegments, unsigned heightSegments,
         float phiStart, float phiLength, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Sphere>;
  static Ptr make(float radius=50,
                  unsigned widthSegments=8,
                  unsigned heightSegments=6,
                  float phiStart=0,
                  float phiLength=(float)M_PI * 2,
                  float thetaStart= 0,
                  float thetaLength=(float)M_PI)
  {
    return Ptr(new Sphere(radius,
                          std::max(widthSegments, 3u),
                          std::max(heightSegments, 2u),
                          phiStart, phiLength, thetaStart, thetaLength));
  }

  Sphere *cloned() const override {
    return new Sphere(*this);
  }
};

}
}
}
#endif //THREEPP_GEOM_SPHERE_H
