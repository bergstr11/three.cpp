//
// Created by byter on 08.08.17.
//

#include "Geometry.h"

namespace three {

Geometry &Geometry::applyMatrix(const math::Matrix4 &matrix)
{
  for(math::Vector3 &vertex : _vertices) {
    vertex.apply(matrix);
  }

  math::Matrix3 normalMatrix = matrix.normalMatrix();
  for (Face3 &face : _faces) {
    face.normal.apply(normalMatrix).normalize();

    for (math::Vector3 &normal : face.vertexNormals) {
      normal.apply(normalMatrix).normalize();
    }
  }

  computeBoundingBox();
  computeBoundingSphere();

  _verticesNeedUpdate = true;
  _normalsNeedUpdate = true;

  return *this;
}

}