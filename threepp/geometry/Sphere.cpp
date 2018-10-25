//
// Created by byter on 11/25/17.
//

#include "Sphere.h"

namespace three {
namespace geometry {

using namespace std;

Sphere::Sphere(float radius, unsigned widthSegments, unsigned heightSegments,
       float phiStart, float phiLength, float thetaStart, float thetaLength)
   : LinearGeometry(mktyper()), SphereParams(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength)
{
  set(buffer::Sphere(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength));
  mergeVertices();
}

namespace buffer {

Sphere::Sphere(float radius, unsigned widthSegments, unsigned heightSegments,
       float phiStart, float phiLength, float thetaStart, float thetaLength)
   : BufferGeometry(mktyper()), SphereParams(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength)
{
  float thetaEnd = thetaStart + thetaLength;

  unsigned index = 0;
  vector<vector<unsigned>> grid;

  // buffers
  unsigned num = (heightSegments + 1) * (widthSegments + 1);

  auto indices = attribute::growing<uint32_t>(true);
  auto vertices = attribute::prealloc<float, Vertex>(num);
  auto normals = attribute::prealloc<float, Vertex>(num, true);
  auto uvs = attribute::prealloc<float, UV>(num, true);

  // generate vertices, normals and uvs
  for (unsigned iy = 0; iy <= heightSegments; iy ++) {

    vector<unsigned> verticesRow;

    float v = (float)iy / heightSegments;

    for (unsigned ix = 0; ix <= widthSegments; ix ++ ) {

      float u = (float)ix / widthSegments;

      // vertex
      float x = - radius * std::cos( phiStart + u * phiLength ) * std::sin( thetaStart + v * thetaLength );
      float y = radius * std::cos( thetaStart + v * thetaLength );
      float z = radius * std::sin( phiStart + u * phiLength ) * std::sin( thetaStart + v * thetaLength );

      vertices->next() = {x, y, z};
      normals->next() = vertices->back().normalized();
      uvs->next() = {u, 1 - v};

      verticesRow.push_back( index ++ );
    }

    grid.push_back( verticesRow );
  }

  // generate indices
  for ( unsigned iy = 0; iy < heightSegments; iy ++ ) {

    for ( unsigned ix = 0; ix < widthSegments; ix ++ ) {

      unsigned a = grid[ iy ][ ix + 1 ];
      unsigned b = grid[ iy ][ ix ];
      unsigned c = grid[ iy + 1 ][ ix ];
      unsigned d = grid[ iy + 1 ][ ix + 1 ];

      if ( iy != 0 || thetaStart > 0 ) {
        indices->next() = a;
        indices->next() = b;
        indices->next() = d;
      }
      if ( iy != heightSegments - 1 || thetaEnd < M_PI ) {
        indices->next() = b;
        indices->next() = c;
        indices->next() = d;
      }
    }
  }

  // build geometry
  setIndex(indices);
  setPosition(vertices);
  setNormal(normals);
  setUV(uvs);
}

}

}
}
