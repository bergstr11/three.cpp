//
// Created by byter on 08.08.17.
//

#ifndef THREE_QT_GEOMETRY_H
#define THREE_QT_GEOMETRY_H

#include <vector>
#include <array>
#include <cstdint>
#include "Face3.h"
#include "math/Vector3.h"
#include "math/Sphere.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include "math/Matrix3.h"
#include "math/Box3.h"
#include "math/Color.h"

namespace three {

class Geometry
{
  std::vector<math::Vector3> _vertices;
  std::vector<math::Color> _colors; // one-to-one vertex colors, used in ParticleSystem, Line and Ribbon

  std::vector<Face3> _faces;

  struct UV {

  };
  std::vector<std::vector<std::array<UV, 4>>> faceVertexUvs;

  struct MorphTarget {

  };
  std::vector<MorphTarget> morphTargets;

  struct MorphNormal {

  };
  std::vector<MorphNormal> morphNormals;

  std::vector<math::Vector4> skinWeights;
  struct SkinIndex
  {
    int x, y, z, w;
  };
  std::vector<SkinIndex> skinIndices;

  math::Box3 _boundingBox;
  math::Sphere _boundingSphere;

  // update flags

  bool _elementsNeedUpdate = false;
  bool _verticesNeedUpdate = false;
  bool _uvsNeedUpdate = false;
  bool _normalsNeedUpdate = false;
  bool _colorsNeedUpdate = false;
  bool _lineDistancesNeedUpdate = false;
  bool _groupsNeedUpdate = false;

  void computeBoundingBox();
  void computeBoundingSphere();

public:
  Geometry &applyMatrix(const math::Matrix4 &matrix);
};

} //three

#endif //THREE_QT_GEOMETRY_H
