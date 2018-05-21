//
// Created by byter on 2/16/18.
//

#include "Polyhedron.h"

namespace three {
namespace geometry {

using namespace std;
using namespace math;

Polyhedron::Polyhedron(const PolyhedronParams &params)
   : PolyhedronParams(params)
{
  set(buffer::Polyhedron(params));
  mergeVertices();
}

namespace buffer {

void correctUV(UV &uv, const Vector3 &vector, float azimuth )
{
  if ( ( azimuth < 0 ) && ( uv.x() == 1 ) ) uv.x() = 0;

  if ( ( vector.x() == 0 ) && ( vector.z() == 0 ) ) uv.x() = azimuth / 2.0f / (float)M_PI + 0.5f;
}

Polyhedron::Polyhedron(const PolyhedronParams &params)
{
  auto vertices = attribute::growing<float, Vertex>(true);
  auto uvs = attribute::growing<float, UV>(true);

  // the subdivision creates the vertex buffer data
  for (unsigned i = 0, l = params.indices.size(); i < l; i += 3 ) {

    // get the vertices of the face
    const Vector3 &a = params.vertices[params.indices[i]];
    const Vector3 &b = params.vertices[params.indices[i+1]];
    const Vector3 &c  = params.vertices[params.indices[i+2]];

    // perform subdivision, construct all of the vertices for this subdivision
    unsigned cols = pow( 2, params.detail );

    // we use this multidimensional array as a data structure for creating the subdivision
    vector<vector<Vertex>> v(cols + 1);

    for (unsigned i = 0; i <= cols; i ++ ) {

      unsigned rows = cols - i;

      v[i].resize(rows + 1);

      Vector3 aj = a.lerped( c, i / cols );
      Vector3 bj = b.lerped( c, i / cols );

      for (unsigned j = 0; j <= rows; j ++ ) {

        if ( j == 0 && i == cols ) {

          v[ i ][ j ] = aj;
        } else {

          v[ i ][ j ] = aj.lerped( bj, j / rows );
        }
      }
    }

    // construct all of the faces
    for (unsigned i = 0; i < cols; i ++ ) {

      for (unsigned j = 0; j < 2 * ( cols - i ) - 1; j ++ ) {

        float k = floor( (float)j / 2 );

        if ( j % 2 == 0 ) {

          vertices->next() = v[ i ][ k + 1 ].normalize() * params.radius;
          vertices->next() = v[ i + 1 ][ k ].normalize() * params.radius;;
          vertices->next() = v[ i ][ k ].normalize() * params.radius;;

        } else {

          vertices->next() = v[ i ][ k + 1 ].normalize() * params.radius;;
          vertices->next() = v[ i + 1 ][ k + 1 ].normalize() * params.radius;;
          vertices->next() = v[ i + 1 ][ k ].normalize() * params.radius;;
        }
      }
    }
  }

  // finally, create the uv data
  for (unsigned i = 0; i < vertices->itemCount(); i ++ ) {

    Vector3 &vertex = vertices->at(i);

    float u = vertex.azimuth() / 2.0f / (float)M_PI + 0.5f;
    float v = vertex.inclination() / (float)M_PI + 0.5f;
    uvs->next() = {u, 1 - v};
  }

  //correctUVs();
  for (unsigned i = 0; i < vertices->itemCount(); i += 3) {

    Vector3 &a = vertices->at(i);
    Vector3 &b = vertices->at(i+1);
    Vector3 &c = vertices->at(i+2);

    UV &uvA = uvs->at(i);
    UV &uvB = uvs->at(i+1);
    UV &uvC = uvs->at(i+2);

    Vector3 centroid = (a + b + c) / 3;

    float azi = centroid.azimuth();

    correctUV( uvA, a, azi );
    correctUV( uvB, b, azi );
    correctUV( uvC, c, azi );
  }

  //correctSeam: handle case when face straddles the seam, see #3269
  for (unsigned i = 0; i < uvs->itemCount(); i += 3 ) {

    // uv data of a single face
    UV &u0 = uvs->at(i);
    UV &u1 = uvs->at(i+1);
    UV &u2 = uvs->at(i+2);

    float xMax = std::max(std::max(u0.x(), u1.x()), u2.x());
    float xMin = std::min(std::min(u0.x(), u1.x()), u2.x());

    // 0.9 is somewhat arbitrary
    if ( xMax > 0.9 && xMin < 0.1 ) {

      if ( u0.x() < 0.2 ) u0.x() += 1;
      if ( u1.x() < 0.2 ) u1.x() += 1;
      if ( u2.x() < 0.2 ) u2.x() += 1;
    }
  }

  // build non-indexed geometry
  setPosition(vertices);
  setNormal(BufferAttributeT<float>::Ptr(vertices->clone()));
  setUV(uvs);

  if (params.detail == 0)
    computeVertexNormals(); // flat normals
  else
    normalizeNormals(); // smooth normals*/
}

}
}
}
