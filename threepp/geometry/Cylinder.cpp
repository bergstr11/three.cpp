//
// Created by byter on 12/21/17.
//

#include "Cylinder.h"

namespace three {
namespace geometry {

using namespace std;

Cylinder::Cylinder(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
                   unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength)
   : CylinderParams(radiusTop, radiusBottom, height, heightSegments, radialSegments, openEnded, thetaStart, thetaLength)
{
  set(buffer::Cylinder(radiusTop, radiusBottom, height, heightSegments, radialSegments, openEnded, thetaStart, thetaLength));
  mergeVertices();
}

namespace buffer {

Cylinder::Cylinder(float radiusTop, float radiusBottom, float height, unsigned heightSegments,
                   unsigned radialSegments, bool openEnded, float thetaStart, float thetaLength)
   : CylinderParams(radiusTop, radiusBottom, height, heightSegments, radialSegments, openEnded, thetaStart, thetaLength)
{
  // buffers
  auto indices = attribute::growing<uint32_t>(false);
  auto vertices = attribute::growing<float, Vertex>(true);
  auto normals = attribute::growing<float, Vertex>(true);
  auto uvs = attribute::growing<float, UV>(true);

  vector<vector<uint32_t>> indexArray;
  float halfHeight = height / 2;
  unsigned index = 0;
  unsigned groupCount = 0, groupStart = 0;

  // generate torso

  // this will be used to calculate the normal
  float slope = (radiusBottom - radiusTop) / height;

  // generate vertices, normals and uvs
  for (unsigned y = 0; y <= heightSegments; y++) {

    vector<uint32_t> indexRow;

    float v = (float) y / heightSegments;

    // calculate the radius of the current row
    float radius = v * (radiusBottom - radiusTop) + radiusTop;

    for (unsigned x = 0; x <= radialSegments; x++) {

      float u = (float) x / radialSegments;

      float theta = u * thetaLength + thetaStart;

      float sinTheta = sin(theta);
      float cosTheta = cos(theta);

      // vertex

      float vx = radius * sinTheta;
      float vy = -v * height + halfHeight;
      float vz = radius * cosTheta;
      vertices->next() = {vx, vy, vz};

      // normal
      Vertex normal(sinTheta, slope, cosTheta);
      normals->next() = normal.normalized();

      // uv
      uvs->next() = {u, 1 - v};

      // save index of vertex in respective row
      indexRow.push_back(index++);
    }

    // now save vertices of the row in our index array
    indexArray.push_back(indexRow);
  }

  // generate indices

  for (unsigned x = 0; x < radialSegments; x++) {

    for (unsigned y = 0; y < heightSegments; y++) {

      // we use the index array to access the correct indices
      unsigned a = indexArray[y][x];
      unsigned b = indexArray[y + 1][x];
      unsigned c = indexArray[y + 1][x + 1];
      unsigned d = indexArray[y][x + 1];

      // faces
      indices->next() = a;
      indices->next() = b;
      indices->next() = d;

      indices->next() = b;
      indices->next() = c;
      indices->next() = d;

      // update group counter
      groupCount += 6;
    }
  }

  // add a group to the geometry. this will ensure multi material support
  addGroup(groupStart, groupCount, 0);

  // calculate new start value for groups
  groupStart += groupCount;

  if (!openEnded) {

    auto generateCap = [&](bool top) {

      //var x, centerIndexStart, centerIndexEnd;

      //var uv = new Vector2();
      //var vertex = new Vector3();

      unsigned groupCount = 0;

      float radius = top ? radiusTop : radiusBottom;
      float sign = top ? 1 : -1;

      // save the index of the first center vertex
      unsigned centerIndexStart = index;

      // first we generate the center vertex data of the cap.
      // because the geometry needs one set of uvs per face,
      // we must generate a center vertex per face/segment

      for (unsigned x = 1; x <= radialSegments; x++) {

        // vertex
        vertices->next() = {0.0f, halfHeight * sign, 0.0f};

        // normal
        normals->next() = {0.0f, sign, 0.0f};

        // uv
        uvs->next() = {0.5f, 0.5f};

        // increase index
        index++;
      }

      // save the index of the last center vertex
      unsigned centerIndexEnd = index;

      // now we generate the surrounding vertices, normals and uvs
      for (unsigned x = 0; x <= radialSegments; x++) {

        float u = (float) x / radialSegments;
        float theta = u * thetaLength + thetaStart;

        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        // vertex
        float vx = radius * sinTheta;
        float vy = halfHeight * sign;
        float vz = radius * cosTheta;
        vertices->next() = {vx, vy, vz};

        // normal
        normals->next() = {0, sign, 0};

        // uv
        float uvx = (cosTheta * 0.5f) + 0.5f;
        float uvy = (sinTheta * 0.5f * sign) + 0.5f;
        uvs->next() = {uvx, uvy};

        // increase index
        index++;
      }

      // generate indices

      for (unsigned x = 0; x < radialSegments; x++) {

        unsigned c = centerIndexStart + x;
        unsigned i = centerIndexEnd + x;

        if (top) {

          // face top
          indices->next() = i;
          indices->next() = i + 1;
          indices->next() = c;

        }
        else {

          // face bottom
          indices->next() = i + 1;
          indices->next() = i;
          indices->next() = c;
        }

        groupCount += 3;
      }

      // add a group to the geometry. this will ensure multi material support
      addGroup(groupStart, groupCount, top ? 1 : 2);

      // calculate new start value for groups
      groupStart += groupCount;
    };

    if (radiusTop > 0) generateCap(true);
    if (radiusBottom > 0) generateCap(false);
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