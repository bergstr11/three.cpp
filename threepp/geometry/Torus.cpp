//
// Created by byter on 2/9/18.
//

#include "Torus.h"

namespace three {
namespace geometry {

using namespace std;

Torus::Torus(const TorusParams &params) : TorusParams(params)
{
  set(buffer::Torus(params));
  mergeVertices();
}

namespace buffer {

Torus::Torus(const TorusParams &params) : TorusParams(params)
{
  size_t num = (radialSegments + 1) * (tubularSegments + 1);

  // buffers
  auto indices = attribute::prealloc<uint32_t>(radialSegments * tubularSegments * 6, true);
  auto vertices = attribute::prealloc<float, Vertex>(num, true);
  auto normals = attribute::prealloc<float, Vertex>(num, true);
  auto uvs = attribute::prealloc<float, UV>(num, true);

  // helper variables

  math::Vector3 center;
  math::Vector3 vertex;
  math::Vector3 normal;

  // generate vertices, normals and uvs
  for (unsigned j = 0; j <= radialSegments; j ++ ) {

    for (unsigned i = 0; i <= tubularSegments; i ++ ) {

      float u = (float)i / tubularSegments * arc;
      float v = (float)j / radialSegments * (float)M_PI * 2;

      // vertex
      vertex.x() = ( radius + tube * cos( v ) ) * cos( u );
      vertex.y() = ( radius + tube * cos( v ) ) * sin( u );
      vertex.z() = tube * sin( v );

      vertices->next() = vertex;

      // normal
      center.x() = radius * cos( u );
      center.y() = radius * sin( u );

      normals->next() = (vertex - center).normalized();

      // uv
      uvs->next() = {(float)i / tubularSegments, (float)j / radialSegments};
    }
  }

  // generate indices
  for (unsigned j = 1; j <= radialSegments; j ++ ) {

    for (unsigned i = 1; i <= tubularSegments; i ++ ) {

      // indices
      unsigned a = ( tubularSegments + 1 ) * j + i - 1;
      unsigned b = ( tubularSegments + 1 ) * ( j - 1 ) + i - 1;
      unsigned c = ( tubularSegments + 1 ) * ( j - 1 ) + i;
      unsigned d = ( tubularSegments + 1 ) * j + i;

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