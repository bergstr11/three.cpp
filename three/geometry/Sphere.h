//
// Created by byter on 11/25/17.
//

#ifndef THREEPP_GEOM_SPHERE_H
#define THREEPP_GEOM_SPHERE_H

#include <core/StaticGeometry.h>
#include <core/BufferGeometry.h>

namespace three {
namespace geometry {

class Sphere : public StaticGeometry
{
  unsigned const _radius, _widthSegments, _heightSegments;
  float _phiStart, _phiLength, _thetaStart, _thetaLength;

protected:
  Sphere(unsigned radius, unsigned widthSegments, unsigned heightSegments,
         float phiStart, float phiLength, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Sphere>;
  static Ptr make(unsigned radius=50,
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

  void toBufferGeometry(BufferGeometry &geometry) override;
};

namespace buffer {

class Sphere : public BufferGeometry
{
friend class three::geometry::Sphere;

  unsigned const _radius, _widthSegments, _heightSegments;
  float _phiStart, _phiLength, _thetaStart, _thetaLength;

protected:
  Sphere(unsigned radius, unsigned widthSegments, unsigned heightSegments,
         float phiStart, float phiLength, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Sphere>;
  static Ptr make(unsigned radius=50,
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
};

}
}
}
#endif //THREEPP_GEOM_SPHERE_H
