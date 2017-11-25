//
// Created by byter on 11/25/17.
//

#include "Sphere.h"

namespace three {
namespace geometry {

using namespace std;

Sphere::Sphere(unsigned radius, unsigned widthSegments, unsigned heightSegments,
       float phiStart, float phiLength, float thetaStart, float thetaLength)
   : _radius(radius), _widthSegments(widthSegments), _heightSegments(heightSegments),
     _phiStart(phiStart), _phiLength(phiLength), _thetaStart(thetaStart), _thetaLength(thetaLength)
{
  set(buffer::Sphere(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength));
}

void Sphere::toBufferGeometry(BufferGeometry &geometry)
{

}

namespace buffer {

Sphere::Sphere(unsigned radius, unsigned widthSegments, unsigned heightSegments,
       float phiStart, float phiLength, float thetaStart, float thetaLength)
   : _radius(radius), _widthSegments(widthSegments), _heightSegments(heightSegments),
     _phiStart(phiStart), _phiLength(phiLength), _thetaStart(thetaStart), _thetaLength(thetaLength)
{
  float thetaEnd = thetaStart + thetaLength;

  unsigned index = 0;
  vector<vector<unsigned>> grid;

  // buffers

  vector<uint32_t> indices;
  vector<Vertex> vertices;
  vector<Vertex> normals;
  vector<UV> uvs;

  // generate vertices, normals and uvs
  for (unsigned iy = 0; iy <= heightSegments; iy ++) {

    vector<unsigned> verticesRow;

    float v = iy / heightSegments;

    for (unsigned ix = 0; ix <= widthSegments; ix ++ ) {

      float u = ix / widthSegments;

      // vertex

      float x = - radius * std::cos( phiStart + u * phiLength ) * std::sin( thetaStart + v * thetaLength );
      float y = radius * std::cos( thetaStart + v * thetaLength );
      float z = radius * std::sin( phiStart + u * phiLength ) * std::sin( thetaStart + v * thetaLength );

      vertices.emplace_back(x, y, z);
      normals.push_back( vertices.back().normalized() );
      uvs.emplace_back(u, 1 - v);

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
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(d);
      }
      if ( iy != heightSegments - 1 || thetaEnd < M_PI ) {
        indices.push_back(b);
        indices.push_back(c);
        indices.push_back(d);
      }
    }
  }

  // build geometry
  setIndex(indices);
  setPosition(BufferAttributeT<float>::make(vertices));
  setNormal(BufferAttributeT<float>::make(normals, true));
  setUV(BufferAttributeT<float>::make(uvs));
}

}

}
}