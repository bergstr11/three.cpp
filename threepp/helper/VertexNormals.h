//
// Created by byter on 4/6/18.
//

#ifndef THREE_PP_VERTEXNORMALS_H
#define THREE_PP_VERTEXNORMALS_H

#include <threepp/objects/Line.h>

namespace three {

class LinearGeometryAccess;
class BufferGeometryAccess;

namespace helper {

struct VertexNormalConfig
{
  size_t size = 1;
  Color color {0xff0000};
  unsigned lineWidth = 1;
};

class VertexNormals : public LineSegments
{
  friend class three::LinearGeometryAccess;
  friend class three::BufferGeometryAccess;

  Object3D::Ptr _object;
  VertexNormalConfig _config;

  static BufferGeometry::Ptr createGeometry(Object3D::Ptr object, float size)
  {
    unsigned nNormals = object->geometry() ? object->geometry()->vertexCount() : 0;

    BufferGeometry::Ptr geometry = BufferGeometry::make();
    auto vertices = attribute::prealloc<float, Vertex>(nNormals * 2);
    geometry->setPosition(vertices);

    return geometry;
  }

  static LineBasicMaterial::Ptr createMaterial(const VertexNormalConfig &config)
  {
    auto ptr = LineBasicMaterial::make();
    ptr->linewidth = config.lineWidth;
    ptr->color = config.color;
    return ptr;
  }

  void update();

  VertexNormals(Object3D::Ptr object, const VertexNormalConfig &config)
     : Object3D(object::ResolverT<LineSegments>::make(*this)),
       LineSegments(createGeometry(object, config.size), createMaterial(config)),
       _object(object), _config(config)
  {
    matrixAutoUpdate = false;
    update();
  }

public:
  using Ptr = std::shared_ptr<VertexNormals>;
  static Ptr make(Object3D::Ptr object, const VertexNormalConfig &options=VertexNormalConfig()) {
    Ptr p(new VertexNormals(object, options));
    return p;
  }

  void setSize(size_t size) {
    if(size != _config.size) {
      _config.size = size;
      update();
    }
  }

  void setLineWidth(unsigned lineWidth) {
    if(lineWidth != _config.lineWidth) {
      _config.lineWidth = lineWidth;
      update();
    }
  }

  void setColor(float redF, float greenF, float blueF) {
    if(redF != _config.color.r || greenF != _config.color.g || blueF != _config.color.b) {
      _config.color.r = redF;
      _config.color.g = greenF;
      _config.color.b = blueF;
      update();
    }
  }
};

}
}

#endif //THREE_PP_VERTEXNORMALS_H
