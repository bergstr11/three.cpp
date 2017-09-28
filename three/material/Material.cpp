//
// Created by byter on 26.09.17.
//

#include <camera/Camera.h>
#include "MeshDistanceMaterial.h"

namespace three {


void MeshDistanceMaterial::setupPointLight(const math::Vector3 &position, float near, float far)
{
  _referencePosition = position;
  _nearDistance = near;
  _farDistance = far;
}

}