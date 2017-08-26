//
// Created by byter on 08.08.17.
//

#include "objects/Mesh.h"

namespace three {

StaticGeometry &StaticGeometry::mergeMesh(const MeshPtr & mesh )
{
  if(mesh->matrixAutoUpdate()) mesh->updateMatrix();

  return merge( mesh->geometry(), mesh->matrix() );
}

void StaticGeometry::addFace(const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t materialIndex)
{

}

}