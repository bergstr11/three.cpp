//
// Created by byter on 11/17/17.
//

#ifndef THREE_QT_DIRECTGEOMETRY_H
#define THREE_QT_DIRECTGEOMETRY_H

#include <helper/Types.h>
#include <math/Sphere.h>
#include "BufferAttribute.h"


namespace three {

class StaticGeometry;

struct DirectGeometry
{
  std::vector<Index> indices;
  std::vector<math::Vector3> vertices;
  std::vector<Color> colors;
  std::vector<Vertex> normals;
  std::vector<UV> uvs;
  std::vector<UV> uv2s;
  std::vector<Group> groups;

  std::vector<std::vector<Vertex>> morphTargetsPosition;
  std::vector<std::vector<Vertex>> morphTargetsNormal;

  std::vector<math::Vector4> skinWeights;
  std::vector<math::Vector4> skinIndices;

  math::Box3 boundingBox;
  math::Sphere boundingSphere;

  // update flags

  bool verticesNeedUpdate = false;
  bool normalsNeedUpdate = false;
  bool colorsNeedUpdate = false;
  bool uvsNeedUpdate = false;
  bool groupsNeedUpdate = false;

  DirectGeometry(const StaticGeometry &geometry);

private:
  void computeGroups(const StaticGeometry &geometry);
};

}
#endif //THREE_QT_DIRECTGEOMETRY_H
