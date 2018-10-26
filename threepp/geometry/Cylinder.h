//
// Created by byter on 12/21/17.
//

#ifndef THREEPP_CYLINDER_H
#define THREEPP_CYLINDER_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

class CylinderParams
{
protected:
  float _radiusTop = 20;
  float _radiusBottom = 20;
  float _height = 100;
  unsigned _heightSegments = 1;
  unsigned _radialSegments = 8;
  bool _openEnded = false;
  float _thetaStart = 0;
  float _thetaLength = (float) M_PI * 2;

  CylinderParams(float radiusTop, float radiusBottom, float height, unsigned heightSegments, unsigned radialSegments,
                 bool openEnded, float thetaStart, float thetaLength) :
     _radiusTop(radiusTop), _radiusBottom(radiusBottom), _height(height), _heightSegments(heightSegments),
     _radialSegments(radialSegments), _openEnded(openEnded), _thetaStart(thetaStart), _thetaLength(thetaLength)
  {}
};

class DLX Cylinder : public CylinderParams, public LinearGeometry
{
  Cylinder(const Cylinder &cylinder) : LinearGeometry(cylinder), CylinderParams(cylinder)
  {}

protected:
  Cylinder(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
           unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Cylinder>;

  static Ptr make(float radiusTop, float radiusBottom,
                  float height,
                  unsigned heightSegments = 1,
                  unsigned radialSegments = 8,
                  bool openEnded = false,
                  float thetaStart = 0.0f,
                  float thetaLength = (float) M_PI * 2)
  {
    return Ptr(new Cylinder(radiusTop, radiusBottom, height, heightSegments, radialSegments,
                            openEnded, thetaStart, thetaLength));
  }

  Cylinder *cloned() const override
  {
    return new Cylinder(*this);
  }
};

namespace buffer {

class DLX Cylinder : public BufferGeometry, public CylinderParams
{
  friend class three::geometry::Cylinder;

  Cylinder(const Cylinder &cylinder) : BufferGeometry(cylinder), CylinderParams(cylinder)
  {}

protected:
  Cylinder(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
           unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Cylinder>;

  static Ptr make(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
                  unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength)
  {
    return Ptr(new Cylinder(radiusTop, radiusBottom, height, heightSegments, radialSegments,
                            openEnded, thetaStart, thetaLength));
  }

  Cylinder *cloned() const override
  {
    return new Cylinder(*this);
  }
};

}

}
}


#endif //THREEPP_CYLINDER_H
