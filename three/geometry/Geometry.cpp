//
// Created by byter on 08.08.17.
//

#include "objects/Mesh.h"

namespace three {

Geometry &Geometry::mergeMesh(const MeshPtr & mesh )
{
  if(mesh->matrixAutoUpdate()) mesh->updateMatrix();

  return merge( mesh->geometry(), mesh->matrix() );
}

}