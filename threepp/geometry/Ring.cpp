//
// Created by byter on 2/8/18.
//

#include "Ring.h"

namespace three {
namespace geometry {

using namespace std;

Ring::Ring(const RingParams &params) : RingParams(params)
{
  RingParams::validate();
  set(buffer::Ring(params));
  mergeVertices();
}

namespace buffer {

Ring::Ring(const RingParams &params) : RingParams(params)
{
  RingParams::validate();
  size_t num = (phiSegments + 1) * (thetaSegments + 1);

  // buffers
  auto indices = attribute::prealloc<uint32_t>(phiSegments * thetaSegments * 6, true);
  auto vertices = attribute::prealloc<float, Vertex>(num, true);
  auto normals = attribute::prealloc<float, Vertex>(num, true);
  auto uvs = attribute::prealloc<float, UV>(num, true);

  float radius = innerRadius;
  float radiusStep = ((outerRadius - innerRadius) / phiSegments);
  math::Vector3 vertex;

  // generate vertices, normals and uvs
  for (unsigned j = 0; j <= phiSegments; j ++ ) {

    for (unsigned i = 0; i <= thetaSegments; i ++ ) {

      // values are generated from the inside of the ring to the outside
      float segment = thetaStart + (float)i / thetaSegments * thetaLength;

      // vertex
      vertex.x() = radius * cos( segment );
      vertex.y() = radius * sin( segment );

      vertices->next() = vertex;

      // normal
      normals->next() = {0.0f, 0.0f, 1.0f};

      //uv
      uvs->next() = {(vertex.x() / outerRadius + 1) / 2, (vertex.y() / outerRadius + 1) / 2};
    }

    // increase the radius for next row of vertices
    radius += radiusStep;
  }

  // indices
  for (unsigned j = 0; j < phiSegments; j ++ ) {

    unsigned thetaSegmentLevel = j * (thetaSegments + 1);

    for (unsigned i = 0; i < thetaSegments; i ++) {

      unsigned segment = i + thetaSegmentLevel;

      size_t a = segment;
      size_t b = segment + thetaSegments + 1;
      size_t c = segment + thetaSegments + 2;
      size_t d = segment + 1;

      // faces
      indices->next() = a;
      indices->next() = b;
      indices->next() = d;
      indices->next() = b;
      indices->next() = c;
      indices->next() = d;
    }
  }

  setIndex(indices);
  setPosition(vertices);
  setNormal(normals);
  setUV(uvs);
}

}

}
}