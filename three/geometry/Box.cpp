//
// Created by byter on 29.07.17.
//

#include "Box.h"
#include <helper/utils.h>

namespace three {
namespace geometry {

Box::Box(unsigned width, unsigned height, unsigned depth,
         unsigned widthSegments, unsigned heightSegments, unsigned depthSegments)
   : _width(width), _height(height), _depth(depth),
     _widthSegments(widthSegments), _heightSegments(heightSegments), _depthSegments(depthSegments)
{
  set(buffer::Box(width, height, depth, widthSegments, heightSegments, depthSegments));
  mergeVertices();
}

namespace buffer {

using El = math::Vector3::Element;

Box::Box(unsigned int width,
         unsigned int height,
         unsigned int depth,
         unsigned widthSegments,
         unsigned heightSegments,
         unsigned depthSegments)
   : _width(width), _height(height), _depth(depth),
     _widthSegments(widthSegments), _heightSegments(heightSegments), _depthSegments(depthSegments)
{
  //_widthSegments = std::floor( widthSegments );
  if (_widthSegments == 0) _widthSegments = 1;
  //_heightSegments = std::floor( heightSegments ) || 1;
  if (_heightSegments == 0) _heightSegments = 1;
  //_depthSegments = std::floor( depthSegments ) || 1;
  if (_depthSegments == 0) _depthSegments = 1;

  // buffers

  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  std::vector<Vertex> normals;
  std::vector<UV> uvs;

  // helper variables

  unsigned numberOfVertices = 0;
  unsigned groupStart = 0;

  auto buildPlane = [&](El u, El v, El w, int udir, int vdir,
                              unsigned width, unsigned height, int depth,
                              unsigned gridX, unsigned gridY, unsigned materialIndex) {

    float segmentWidth = (float)width / gridX;
    float segmentHeight = (float)height / gridY;

    float widthHalf = (float)width / 2;
    float heightHalf = (float)height / 2;
    float depthHalf = (float)depth / 2;

    unsigned gridX1 = gridX + 1;
    unsigned gridY1 = gridY + 1;

    unsigned vertexCounter = 0;
    unsigned groupCount = 0;

    // generate vertices, normals and uvs

    for (unsigned iy = 0; iy < gridY1; iy++) {

      float y = iy * segmentHeight - heightHalf;

      for (unsigned ix = 0; ix < gridX1; ix++) {

        float x = ix * segmentWidth - widthHalf;

        // set values to correct vector component
        math::Vector3 vector;
        vector[u] = x * udir;
        vector[v] = y * vdir;
        vector[w] = depthHalf;

        // now apply vector to vertex buffer
        vertices.push_back(vector);

        // set values to correct vector component
        vector[u] = 0;
        vector[v] = 0;
        vector[w] = depth > 0 ? 1 : -1;

        // now apply vector to normal buffer
        normals.push_back(vector);

        // uvs
        uvs.emplace_back((float)ix / gridX, (float)1 - ((float)iy / gridY));

        // counters
        vertexCounter += 1;
      }
    }

    // indices

    // 1. you need three indices to draw a single face
    // 2. a single segment consists of two faces
    // 3. so we need to generate six (2*3) indices per segment

    for (unsigned iy = 0; iy < gridY; iy++) {

      for (unsigned ix = 0; ix < gridX; ix++) {

        uint32_t a = numberOfVertices + ix + gridX1 * iy;
        uint32_t b = numberOfVertices + ix + gridX1 * (iy + 1);
        uint32_t c = numberOfVertices + (ix + 1) + gridX1 * (iy + 1);
        uint32_t d = numberOfVertices + (ix + 1) + gridX1 * iy;

        // faces
        indices += {a, b, d};
        indices += {b, c, d};

        // increase counter
        groupCount += 6;
      }
    }

    // add a group to the geometry. this will ensure multi material support
    addGroup(groupStart, groupCount, materialIndex);

    // calculate new start value for groups
    groupStart += groupCount;

    // update total number of vertices
    numberOfVertices += vertexCounter;
  };

  // build each side of the box geometry
  buildPlane(El::z, El::y, El::x, -1, -1, depth, height, width, depthSegments, heightSegments, 0); // px
  buildPlane(El::z, El::y, El::x, 1, -1, depth, height, -width, depthSegments, heightSegments, 1); // nx
  buildPlane(El::x, El::z, El::y, 1, 1, width, depth, height, widthSegments, depthSegments, 2); // py
  buildPlane(El::x, El::z, El::y, 1, -1, width, depth, -height, widthSegments, depthSegments, 3); // ny
  buildPlane(El::x, El::y, El::z, 1, -1, width, height, depth, widthSegments, heightSegments, 4); // pz
  buildPlane(El::x, El::y, El::z, -1, -1, width, height, -depth, widthSegments, heightSegments, 5); // nz

  // build geometry

  setIndex(DefaultBufferAttribute<uint32_t>::make(indices, 1, true));
  setPosition(DefaultBufferAttribute<float>::make(vertices));
  setNormal(DefaultBufferAttribute<float>::make(normals, true));
  setUV(DefaultBufferAttribute<float>::make(uvs));
}

}
}
}