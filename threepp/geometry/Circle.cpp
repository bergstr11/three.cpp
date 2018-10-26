//
// Created by byter on 12/21/17.
//

#include "Circle.h"

namespace three {
namespace geometry {

Circle::Circle(float radius, unsigned segments, float thetaStart, float thetaLength)
   : CircleParams(radius, segments, thetaStart, thetaLength)
{
  set(buffer::Circle(radius, segments, thetaStart, thetaLength));
  mergeVertices();
}

namespace buffer {

Circle::Circle(const Circle &circle) : BufferGeometry(circle), CircleParams(circle) {}

Circle::Circle(float radius, unsigned segments, float thetaStart, float thetaLength)
  : CircleParams(radius, segments, thetaStart, thetaLength)
{
  segments = std::max(3u, segments);

  // buffers
  auto indices = attribute::prealloc<uint32_t>(segments * 3, true);
  auto vertices = attribute::prealloc<float, Vertex>(segments + 2, true);
  auto normals = attribute::prealloc<float, Vertex>(segments + 2, true);
  auto uvs = attribute::prealloc<float, UV>(segments + 2, true);

  // center point
  vertices->next() = {0, 0, 0};
  normals->next() = {0, 0, 1};
  uvs->next() = {0.5, 0.5};

  for (unsigned s = 0; s <= segments; s ++ ) {

    float segment = thetaStart + s / segments * thetaLength;

    // vertex
    float x = radius * std::cos( segment );
    float y = radius * std::sin( segment );

    vertices->next() = {x, y, 0};

    // normal
    normals->next() = {0, 0, 1};

    // uvs
    float uvx = (x / radius + 1) / 2;
    float uvy = (y / radius + 1) / 2;

    uvs->next() = {uvx, uvy};
  }

  // indices
  for (uint32_t i = 1; i <= segments; i ++ ) {
    indices->next() = i;
    indices->next() = i + 1;
    indices->next() = 0u;
  }

  setIndex(indices);
  setPosition(vertices);
  setNormal(normals);
  setUV(uvs);
}

}

}
}