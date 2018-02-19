//
// Created by byter on 2/16/18.
//

#ifndef THREE_PP_OCTAHEDRON_H
#define THREE_PP_OCTAHEDRON_H

#include <threepp/geometry/Polyhedron.h>

namespace three {
namespace geometry {

class Octahedron : public LinearGeometry
{
protected:
  Octahedron(float radius, unsigned detail);

public:
  using Ptr = std::shared_ptr<Octahedron>;
  static Ptr make(float radius, unsigned detail) {
    return Ptr(new Octahedron(radius, detail));
  }
};

namespace buffer {

class Octahedron : public Polyhedron
{
  friend class three::geometry::Octahedron;

  static const PolyhedronParams makeParams(float radius, unsigned detail);

protected:
  Octahedron(float radius, unsigned detail) : Polyhedron(makeParams(radius, detail)) {}

public:
  using Ptr = std::shared_ptr<Octahedron>;
  static Ptr make(float radius, unsigned detail) {
    return Ptr(new Octahedron(radius, detail));
  }
};

}
}
}


#endif //THREE_PP_OCTAHEDRON_H
