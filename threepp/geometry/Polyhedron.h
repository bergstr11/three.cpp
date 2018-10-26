//
// Created by byter on 2/16/18.
//

#ifndef THREE_PP_POLYHEDRON_H
#define THREE_PP_POLYHEDRON_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

class PolyhedronParams
{
protected:
  std::vector<Vertex> _vertices;
  std::vector<unsigned> _indices;
  float _radius = 1.0f;
  unsigned _detail = 0;

public:
  PolyhedronParams(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices, float radius, unsigned detail)
  : _vertices(vertices), _indices(indices), _radius(radius), _detail(detail) {}
};

class DLX Polyhedron : public LinearGeometry, public PolyhedronParams
{
protected:
  Polyhedron(const Polyhedron &polyhedron) : LinearGeometry(polyhedron), PolyhedronParams(polyhedron) {}
  explicit Polyhedron(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices, float radius, unsigned detail);

public:
  using Ptr = std::shared_ptr<Polyhedron>;
  static Ptr make(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices, float radius = 1.0f, unsigned detail = 0) {
    return Ptr(new Polyhedron(vertices, indices, radius, detail));
  }

  Polyhedron *cloned() const override {
    return new Polyhedron(*this);
  }
};

namespace buffer {

class DLX Polyhedron : public BufferGeometry, public PolyhedronParams
{
  friend class three::geometry::Polyhedron;

protected:
  Polyhedron(const Polyhedron &polyhedron) : BufferGeometry(polyhedron), PolyhedronParams(polyhedron) {}
  explicit Polyhedron(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices, float radius, unsigned detail);

  void subdivideFace(const math::Vector3 &a, const math::Vector3 &b, const math::Vector3 &c, unsigned detail);

public:
  using Ptr = std::shared_ptr<Polyhedron>;
  static Ptr make(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices, float radius = 1.0f, unsigned detail = 0) {
    return Ptr(new Polyhedron(vertices, indices, radius, detail));
  }

  Polyhedron *cloned() const override {
    return new Polyhedron(*this);
  }
};

}
}
}


#endif //THREE_PP_POLYHEDRON_H
