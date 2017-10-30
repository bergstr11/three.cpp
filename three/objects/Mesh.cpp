//
// Created by byter on 29.07.17.
//

#include "Mesh.h"
#include <math/Matrix4.h>
#include <math/Sphere.h>
#include <math/Triangle.h>
#include <core/Raycaster.h>

namespace three {

void Mesh::raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const
{
  if (materialCount() == 0) return;

  // Checking boundingSphere distance to ray
  if (geometry()->boundingSphere().isEmpty()) geometry()->computeBoundingSphere();

  math::Sphere sphere = geometry()->boundingSphere();
  sphere.apply(_matrixWorld);

  if (!raycaster.ray().intersectsSphere(sphere)) return;

  math::Matrix4 inverseMatrix = _matrixWorld.inverse();
  math::Ray ray = raycaster.ray() * inverseMatrix;

  // Check boundingBox before continuing
  if (!geometry()->boundingBox().isEmpty()) {
    if (!ray.intersectsBox(geometry()->boundingBox())) return;
  }

  math::Vector3 intersectionPoint;
  math::Vector3 intersectionPointWorld;

  geometry()->raycast(*this, raycaster, ray, intersectionPoint, intersectionPointWorld, intersects);
}

}