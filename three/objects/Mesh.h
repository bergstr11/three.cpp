//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_MESH
#define THREEQT_MESH

#include <core/Object3D.h>

namespace three {

class Mesh : public Object3D
{
public:
  using Ptr = std::shared_ptr<Mesh>;
  //Geometry::Ptr _geometry;
};

}


#endif //THREEQT_MESH
