//
// Created by byter on 2/16/18.
//

#include "Octahedron.h"

namespace three {
namespace geometry {

Octahedron::Octahedron(float radius, unsigned detail)
{
  set(buffer::Octahedron(radius, detail));
  mergeVertices();
}

namespace buffer {

const PolyhedronParams Octahedron::makeParams(float radius, unsigned detail)
{
  static const std::vector<math::Vector3> vertices {
     {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
     {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
  };

  static const std::vector<uint32_t> indices = {
     0, 2, 4, 0, 4, 3, 0, 3, 5,
     0, 5, 2, 1, 2, 5, 1, 5, 3,
     1, 3, 4, 1, 4, 2
  };

  PolyhedronParams params;
  params.radius = radius;
  params.detail = detail;
  params.vertices = vertices;
  params.indices = indices;

  return params;
}

}
}
}