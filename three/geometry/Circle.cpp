//
// Created by byter on 12/21/17.
//

#include "Circle.h"

namespace three {
namespace geometry {

Circle::Circle(float radius, unsigned segments, float thetaStart, float thetaLength)
   : _radius(radius), _segments(segments), _thetaStart(thetaStart), _thetaLength(thetaLength)
{
  set(buffer::Circle(radius, segments, thetaStart, thetaLength));
  mergeVertices();
}

namespace buffer {

Circle::Circle(float radius, unsigned segments, float thetaStart, float thetaLength)
{
  segments = std::max(3u, segments);

  // buffers
  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  std::vector<Vertex> normals;
  std::vector<UV> uvs;

  // center point
  vertices.emplace_back(0, 0, 0);
  normals.emplace_back(0, 0, 1);
  uvs.emplace_back(0.5, 0.5);

  for (unsigned s = 0, i = 3; s <= segments; s ++, i += 3 ) {

    float segment = thetaStart + s / segments * thetaLength;

    // vertex
    float x = radius * std::cos( segment );
    float y = radius * std::sin( segment );

    vertices.emplace_back( x, y, 0 );

    // normal
    normals.emplace_back(0, 0, 1);

    // uvs
    float uvx = (x / radius + 1) / 2;
    float uvy = (y / radius + 1) / 2;

    uvs.emplace_back(uvx, uvy);
  }

  // indices
  for (uint32_t i = 1; i <= segments; i ++ ) {
    indices += {i, i + 1, 0u};
  }

  // build geometry
  setIndex(DefaultBufferAttribute<uint32_t>::make(indices, 1, true));
  setPosition(DefaultBufferAttribute<float>::make(vertices));
  setNormal(DefaultBufferAttribute<float>::make(normals));
  setUV(DefaultBufferAttribute<float>::make(uvs));
}

}

}
}