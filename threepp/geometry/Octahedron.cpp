//
// Created by byter on 2/16/18.
//

#include "Octahedron.h"

namespace three {
namespace geometry {

static const std::vector<math::Vector3> octVertices {
   {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
   {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
};

static const std::vector<uint32_t> octIndices = {
   0, 2, 4, 0, 4, 3, 0, 3, 5,
   0, 5, 2, 1, 2, 5, 1, 5, 3,
   1, 3, 4, 1, 4, 2
};

Octahedron::Octahedron(float radius, unsigned detail) : Polyhedron(octVertices, octIndices, radius, detail) {}

namespace buffer {

Octahedron::Octahedron(float radius, unsigned detail) : Polyhedron(octVertices, octIndices, radius, detail) {}

}
}
}