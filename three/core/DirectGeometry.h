//
// Created by byter on 11/17/17.
//

#ifndef THREE_QT_DIRECTGEOMETRY_H
#define THREE_QT_DIRECTGEOMETRY_H

#include <helper/Types.h>
#include <math/Sphere.h>
#include "BufferAttribute.h"
#include "StaticGeometry.h"

namespace three {

class DirectGeometry : public Geometry
{
  friend class BufferGeometry;
protected:
  DirectGeometry(const StaticGeometry &geometry);
  void computeGroups(const StaticGeometry &geometry);

public:
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

  // update flags

  bool verticesNeedUpdate = false;
  bool normalsNeedUpdate = false;
  bool colorsNeedUpdate = false;
  bool uvsNeedUpdate = false;
  bool groupsNeedUpdate = false;

  using Ptr = std::shared_ptr<DirectGeometry>;
  static Ptr make(const StaticGeometry &geometry) {
    return Ptr(new DirectGeometry(geometry));
  }

  void toBufferGeometry(BufferGeometry &geometry) override
  {
  }

  DirectGeometry &apply(const math::Matrix4 &matrix) override
  {
    math::Matrix3 normalMatrix = matrix.normalMatrix();

    for(Vertex &vertex : vertices) {
      vertex.apply( matrix );
    }

    computeBoundingBox();
    computeBoundingSphere();

    verticesNeedUpdate = true;
    normalsNeedUpdate = true;

    return *this;
  }

  bool useMorphing() const override
  {
    return false;
  }

  DirectGeometry &computeBoundingBox() override
  {
    _boundingBox.set(vertices);
    return *this;
  }

  DirectGeometry &computeBoundingSphere() override
  {
    _boundingSphere.set(vertices);
    return *this;
  }
};

}
#endif //THREE_QT_DIRECTGEOMETRY_H
