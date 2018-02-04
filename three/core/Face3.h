//
// Created by byter on 08.08.17.
//

#ifndef THREE_QT_FACE3_H
#define THREE_QT_FACE3_H

#include <array>
#include <three/math/Vector3.h>
#include "Color.h"

namespace three {

using Vertex = math::Vector3;

struct Face3
{
  union {
    struct { unsigned a, b, c; };
    unsigned elements[3];
  };

  Vertex normal;
  std::vector<Vertex> vertexNormals;

  Color color {ColorName::white};
  std::vector<Color> vertexColors;

  unsigned materialIndex;

  Face3() : a(0), b(0), c(0), materialIndex(0) {}

  Face3(unsigned a, unsigned b, unsigned c, const Vertex &normal, unsigned materialIndex=0) :
     a(a), b(b), c(c), normal(normal), materialIndex(materialIndex)
  {
  }

  Face3(unsigned a, unsigned b, unsigned c,
        const std::vector<Vertex> &normal, const std::vector<Color> &color, unsigned materialIndex=0) :
     a(a), b(b), c(c), materialIndex(materialIndex)
  {
    vertexNormals.insert(vertexNormals.begin(), normal.cbegin(), normal.cend());
    vertexColors.insert(vertexColors.begin(), color.cbegin(), color.cend());
  }
};

}


#endif //THREE_QT_FACE3_H
