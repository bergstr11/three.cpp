//
// Created by byter on 08.08.17.
//

#ifndef THREE_QT_FACE3_H
#define THREE_QT_FACE3_H

#include <array>
#include "math/Vector3.h"
#include "Color.h"

namespace three {

using Vertex = math::Vector3;

struct Face3
{
  union {
    struct { int a, b, c, d; };
    int elements[4];
  };

  Vertex normal;
  std::array<Vertex, 3> vertexNormals;

  Color color;
  std::array<Color, 3> vertexColors;

  unsigned materialIndex = 0;

  Face3(unsigned a, unsigned b, unsigned c, Vertex normal=Vertex(), Color color=Color(), unsigned materialIndex=0) :
     a(a), b(b), c(c), normal(normal), color(color), materialIndex(materialIndex)
  {
    vertexNormals[0] = normal;
    vertexColors[0] = color;
  }
};

}


#endif //THREE_QT_FACE3_H
