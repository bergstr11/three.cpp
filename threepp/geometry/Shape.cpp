//
// Created by byter on 8/11/18.
//

#include "Shape.h"
#include <algorithm>
#include <threepp/extras/ShapeUtils.h>

namespace three {
namespace geometry {

using namespace std;
namespace utils = extras::shapeutils;

Shape::Shape(const std::vector<extras::Shape::Ptr> &shapes, unsigned curveSegments) {
  set(buffer::Shape(shapes, curveSegments));
  mergeVertices();
}

namespace buffer {

void addShape(const extras::Shape::Ptr shape,
              unsigned curveSegments,
              attribute::growing_t<uint32_t> &indices,
              attribute::growing_t<float, Vertex> &vertices,
              attribute::growing_t<float, Vertex> &normals,
              attribute::growing_t<float, UV> &uvs,
              unsigned &groupCount)

{
  //var i, l, shapeHole;

  unsigned indexOffset = vertices->itemCount();

  std::vector<math::Vector2> shapeVertices;
  std::vector<std::vector<math::Vector2>> shapeHoles;
  shape->extractPoints(curveSegments, shapeVertices, shapeHoles);

  // check direction of vertices
  if (!utils::isClockWise(shapeVertices)) {

    reverse(shapeVertices.begin(), shapeVertices.end());

    // also check if holes are in the opposite direction
    for (unsigned i = 0, l = shapeHoles.size(); i < l; i++) {

      auto &shapeHole = shapeHoles[i];

      if (utils::isClockWise(shapeHole)) {

        reverse(shapeHoles[i].begin(), shapeHoles[i].end());
      }
    }
  }

  vector<utils::Face> faces = utils::triangulateShape(shapeVertices, shapeHoles);

  // join vertices of inner and outer paths to a single array
  for (const auto &shapeHole : shapeHoles) {

    shapeVertices.insert(shapeVertices.begin(), shapeHole.begin(), shapeHole.end());
  }

  // vertices, normals, uvs
  for (const auto &vertex : shapeVertices) {

    vertices->next() = {vertex.x(), vertex.y(), 0};
    normals->next() = {0, 0, 1};
    uvs->next() = {vertex.x(), vertex.y()}; // world uvs
  }

  // incides
  for (const auto &face : faces) {

    indices->next() = face[0] + indexOffset;
    indices->next() = face[1] + indexOffset;
    indices->next() = face[2] + indexOffset;

    groupCount += 3;
  }
}

Shape::Shape(const std::vector<extras::Shape::Ptr> &shapes, unsigned curveSegments)
{
  auto indices = attribute::growing<uint32_t>(true);
  auto vertices = attribute::growing<float, Vertex>();
  auto normals = attribute::growing<float, Vertex>(true);
  auto uvs = attribute::growing<float, UV>(true);

  // helper variables
  unsigned groupStart = 0;
  unsigned groupCount = 0;
  unsigned index = 0;

  for (const auto &shape : shapes) {

    addShape(shape, curveSegments, indices, vertices, normals, uvs, groupCount);

    addGroup(groupStart, groupCount, index); // enables MultiMaterial support

    groupStart += groupCount;
    groupCount = 0;
    index++;
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