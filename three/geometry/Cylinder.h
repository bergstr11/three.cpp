//
// Created by byter on 12/21/17.
//

#ifndef THREEPP_CYLINDER_H
#define THREEPP_CYLINDER_H

#include <core/LinearGeometry.h>
#include <core/BufferGeometry.h>

namespace three {
namespace geometry {

struct CylinderParams
{
  float radiusTop = 20;
  float radiusBottom = 20;
  float height = 100;
  unsigned heightSegments = 1;
  unsigned radialSegments = 8;
  bool openEnded = false;
  float thetaStart = 0;
  float thetaLength = (float)M_PI * 2;
};

class Cylinder : public LinearGeometry
{
  float _radiusTop;
  float _radiusBottom;
  float _height;
  unsigned _heightSegments;
  unsigned _radialSegments;
  bool _openEnded;
  float _thetaStart;
  float _thetaLength;

  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  std::vector<Vertex> normals;
  std::vector<UV> uvs;

protected:
  Cylinder(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
           unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Cylinder>;

  static Ptr make(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
                  unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength) {
    return Ptr(new Cylinder(radiusTop, radiusBottom, height, heightSegments, radialSegments,
                            openEnded, thetaStart, thetaLength));
  }

  static Ptr make(const CylinderParams &params=CylinderParams()) {
    return Ptr(new Cylinder(params.radiusTop, params.radiusBottom, params.height, params.heightSegments,
                            params.radialSegments, params.openEnded, params.thetaStart, params.thetaLength));
  }
};

namespace buffer {

class Cylinder : public BufferGeometry
{
  friend class three::geometry::Cylinder;

protected:
  Cylinder(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
           unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Cylinder>;

  static Ptr make(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
                  unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength) {
    return Ptr(new Cylinder(radiusTop, radiusBottom, height, heightSegments, radialSegments,
                            openEnded, thetaStart, thetaLength));
  }

  static Ptr make(const CylinderParams &params=CylinderParams()) {
    return Ptr(new Cylinder(params.radiusTop, params.radiusBottom, params.height, params.heightSegments,
                            params.radialSegments, params.openEnded, params.thetaStart, params.thetaLength));
  }
};

}

}
}


#endif //THREEPP_CYLINDER_H
