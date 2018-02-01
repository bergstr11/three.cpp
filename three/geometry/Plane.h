//
// Created by byter on 20.08.17.
//

#ifndef THREE_QT_PLANE_H
#define THREE_QT_PLANE_H

#include <cstdint>
#include "core/LinearGeometry.h"
#include "core/BufferGeometry.h"

namespace three {
namespace geometry {

struct PlaneParameters
{
  float width;
  float height;
  float widthSegments;
  float heightSegments;

  PlaneParameters(float width, float height, float widthSegments, float heightSegments)
     : width(width), height(height), widthSegments(widthSegments),
       heightSegments(heightSegments)
  {}
};

namespace buffer {

class Plane : public BufferGeometry, public PlaneParameters
{
public:
  Plane(float width, float height, float widthSegments=1, float heightSegments=1)
     : PlaneParameters(width, height, widthSegments, heightSegments)
  {
    float width_half = width / 2;
    float height_half = height / 2;

    unsigned gridX = std::max((unsigned)std::floor(widthSegments), 1u);
    unsigned gridY = std::max((unsigned)std::floor(heightSegments), 1u);

    unsigned gridX1 = gridX + 1;
    unsigned gridY1 = gridY + 1;

    float segment_width = width / gridX;
    float segment_height = height / gridY;

    // buffers
    std::vector<uint32_t> indices(gridX1 * gridY1 * 6);
    indices.clear();
    std::vector<Vertex> vertices(gridX1 * gridY1 * 3);
    vertices.clear();
    std::vector<Vertex> normals(gridX1 * gridY1 * 3);
    normals.clear();
    std::vector<UV> uvs(gridX1 * gridY1 * 2);
    uvs.clear();

    // generate vertices, normals and uvs
    for (unsigned iy = 0; iy < gridY1; iy ++) {
      float y = (float)iy * segment_height - height_half;

      for (unsigned ix = 0; ix < gridX1; ix ++ ) {
        float x = (float)ix * segment_width - width_half;

        vertices.emplace_back(x, -y, 0);

        normals.emplace_back(0, 0, 1);

        uvs.emplace_back((float)ix / gridX, 1.0f - ((float)iy / gridY));
      }
    }

    // indices
    for (unsigned iy = 0; iy < gridY; iy ++ ) {
      for (unsigned ix = 0; ix < gridX; ix ++ ) {

        uint32_t a = ix + gridX1 * iy;
        uint32_t b = ix + gridX1 * ( iy + 1 );
        uint32_t c = ( ix + 1 ) + gridX1 * ( iy + 1 );
        uint32_t d = ( ix + 1 ) + gridX1 * iy;

        // faces
        for(auto i : {a, b, d, b, c, d}) indices.push_back(i);
      }
    }

    // build geometry
    setIndex(DefaultBufferAttribute<uint32_t>::make(indices, 1, true));
    setPosition(DefaultBufferAttribute<float>::make(vertices));
    setNormal(DefaultBufferAttribute<float>::make(normals, true));
    setUV(DefaultBufferAttribute<float>::make(uvs));
  }

  using Ptr = std::shared_ptr<Plane>;
  static Ptr make(float width, float height, float widthSegments=1, float heightSegments=1) {
    return Ptr(new Plane(width, height, widthSegments, heightSegments));
  }
};

}

class Plane : public LinearGeometry, public PlaneParameters
{
protected:
  Plane(float width, float height, float widthSegments, float heightSegments)
     : PlaneParameters(width, height, widthSegments, heightSegments)
  {
    set(buffer::Plane(width, height, widthSegments, heightSegments));
    mergeVertices();
  }

public:
  using Ptr = std::shared_ptr<Plane>;
  static Ptr make(float width, float height, float widthSegments, float heightSegments)
  {
    return Ptr(new Plane(width, height, widthSegments, heightSegments));
  }
};

}
}

#endif //THREE_QT_PLANE_H
