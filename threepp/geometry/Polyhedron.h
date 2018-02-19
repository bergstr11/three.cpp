//
// Created by byter on 2/16/18.
//

#ifndef THREE_PP_POLYHEDRON_H
#define THREE_PP_POLYHEDRON_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

struct PolyhedronParams
{
  std::vector<Vertex> vertices;
  std::vector<unsigned> indices;
  float radius = 1.0f;
  unsigned detail = 0;
};

class Polyhedron : public LinearGeometry, private PolyhedronParams
{
protected:
  Polyhedron(const PolyhedronParams &params);

public:
  using Ptr = std::shared_ptr<Polyhedron>;
  static Ptr make(const PolyhedronParams &params) {
    return Ptr(new Polyhedron(params));
  }
};

namespace buffer {

class Polyhedron : public BufferGeometry
{
  friend class three::geometry::Polyhedron;

protected:
  Polyhedron(const PolyhedronParams &params);

  void subdivideFace(const math::Vector3 &a, const math::Vector3 &b, const math::Vector3 &c, unsigned detail);

public:
  using Ptr = std::shared_ptr<Polyhedron>;
  static Ptr make(const PolyhedronParams &params) {
    return Ptr(new Polyhedron(params));
  }
};

}
}
}


#endif //THREE_PP_POLYHEDRON_H
