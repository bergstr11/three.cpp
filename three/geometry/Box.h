//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_BOX
#define THREEQT_BOX

#include "core/StaticGeometry.h"

namespace three {
namespace geometry {

class Box : public StaticGeometry
{
public:
  Box(unsigned width, unsigned height, unsigned depth);
};

}
}


#endif //THREEQT_BOX
