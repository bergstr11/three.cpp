//
// Created by byter on 08.08.17.
//

#ifndef THREE_QT_FACE3_H
#define THREE_QT_FACE3_H

#include <vector>
#include "math/Vector3.h"

namespace three {

struct Face3
{
  unsigned a, b, c;

  math::Vector3 normal;
  std::vector<math::Vector3> vertexNormals;

  //Color color;
  //std::vector<Color> vertexColors;

  unsigned materialIndex = 0;
};

}


#endif //THREE_QT_FACE3_H
