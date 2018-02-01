//
// Created by byter on 11/17/17.
//

#include "DirectGeometry.h"
#include "LinearGeometry.h"

namespace three {

using namespace std;

void DirectGeometry::computeGroups(const LinearGeometry &geometry)
{
  unsigned materialIndex = std::numeric_limits<unsigned>::max();

  const auto &faces = geometry._faces;

  for (unsigned i = 0; i < faces.size(); i ++ ) {

    const auto &face = faces[ i ];

    // materials
    if ( face.materialIndex != materialIndex ) {

      materialIndex = face.materialIndex;

      if (!groups.empty()) {
        Group &group = groups.back();
        group.count = ( i * 3 ) - group.start;
      }

      groups.emplace_back(i * 3, 0, materialIndex);
    }
  }

  if (!groups.empty()) {
    Group &group = groups.back();
    group.count = ((unsigned)faces.size() * 3) - group.start;
  }
}

DirectGeometry::DirectGeometry(const LinearGeometry &geometry)
{
  const auto &faces = geometry._faces;
  const auto &vertices = geometry._vertices;
  const auto &faceVertexUvs = geometry._faceVertexUvs;

  bool hasFaceVertexUv = faceVertexUvs[ 0 ].size() > 0;
  bool hasFaceVertexUv2 = faceVertexUvs[ 1 ].size() > 0;

  // morphs

  const auto &morphTargets = geometry._morphTargets;
  size_t morphTargetsLength = morphTargets.size();
  morphTargetsPosition.resize(morphTargetsLength);

  const auto &morphNormals = geometry._morphNormals;
  size_t morphNormalsLength = morphNormals.size();
  morphTargetsNormal.resize(morphNormalsLength);

  // skins

  const auto &skinIndices = geometry._skinIndices;
  const auto &skinWeights = geometry._skinWeights;

  bool hasSkinIndices = skinIndices.size() == vertices.size();
  bool hasSkinWeights = skinWeights.size() == vertices.size();

  //
  for (size_t i = 0; i < faces.size(); i ++ ) {

    const Face3 &face = faces[ i ];

    this->vertices.push_back(vertices[ face.a ]);
    this->vertices.push_back(vertices[ face.b ]);
    this->vertices.push_back(vertices[ face.c ]);

    const auto &vertexNormals = face.vertexNormals;

    if ( vertexNormals.size() == 3 ) {

      this->normals.push_back(vertexNormals[ 0 ]);
      this->normals.push_back(vertexNormals[ 1 ]);
      this->normals.push_back(vertexNormals[ 2 ]);

    } else {
      const auto &normal = face.normal;

      this->normals.push_back(normal);
      this->normals.push_back(normal);
      this->normals.push_back(normal);
    }

    const auto &vertexColors = face.vertexColors;

    if ( vertexColors.size() == 3 ) {

      this->colors.push_back(vertexColors[ 0 ]);
      this->colors.push_back(vertexColors[ 1 ]);
      this->colors.push_back(vertexColors[ 2 ]);
    }
    else {
      const auto &color = face.color;
      this->colors.push_back(color);
      this->colors.push_back(color);
      this->colors.push_back(color);
    }

    if (hasFaceVertexUv) {

      const std::vector<UV_Array> &uvs = faceVertexUvs[0];

      if (uvs.size() > i) {
        const UV_Array &vertexUvs = uvs[i];

        this->uvs.push_back(vertexUvs[ 0 ]);
        this->uvs.push_back(vertexUvs[ 1 ]);
        this->uvs.push_back(vertexUvs[ 2 ]);
      }
      else {
        //console.warn( 'THREE.DirectGeometry.fromGeometry(): Undefined vertexUv ', i );
        this->uvs.resize(3);
      }
    }

    if (hasFaceVertexUv2) {

      const std::vector<UV_Array> &uvs2 = faceVertexUvs[1];

      if (uvs2.size() > i) {
        const UV_Array &vertexUvs = uvs2[i];

        this->uvs.push_back(vertexUvs[ 0 ]);
        this->uvs.push_back(vertexUvs[ 1 ]);
        this->uvs.push_back(vertexUvs[ 2 ]);
      }
      else {
        //console.warn( 'THREE.DirectGeometry.fromGeometry(): Undefined vertexUv2 ', i );
        this->uvs.resize(3);
      }
    }

    // morphs

    for ( size_t j = 0; j < morphTargetsLength; j ++ ) {

      const auto &morphTarget = morphTargets[ j ].vertices;

      morphTargetsPosition[j].push_back(morphTarget[ face.a ]);
      morphTargetsPosition[j].push_back(morphTarget[ face.b ]);
      morphTargetsPosition[j].push_back(morphTarget[ face.c ]);
    }

    for ( size_t j = 0; j < morphNormalsLength; j ++ ) {

      const auto &morphNormal = morphNormals[ j ].vertexNormals[ i ];

      morphTargetsNormal[j].push_back(morphNormal[0]);
      morphTargetsNormal[j].push_back(morphNormal[1]);
      morphTargetsNormal[j].push_back(morphNormal[2]);
    }

    // skins
    if ( hasSkinIndices ) {
      this->skinIndices.push_back(skinIndices[face.a]);
      this->skinIndices.push_back(skinIndices[face.b]);
      this->skinIndices.push_back(skinIndices[face.c]);
    }

    if ( hasSkinWeights ) {
      this->skinWeights.push_back(skinWeights[face.a]);
      this->skinWeights.push_back(skinWeights[face.b]);
      this->skinWeights.push_back(skinWeights[face.c]);
    }
  }

  computeGroups( geometry );

  verticesNeedUpdate = geometry._verticesNeedUpdate;
  normalsNeedUpdate = geometry._normalsNeedUpdate;
  colorsNeedUpdate = geometry._colorsNeedUpdate;
  uvsNeedUpdate = geometry._uvsNeedUpdate;
  groupsNeedUpdate = geometry._groupsNeedUpdate;
}


}