//
// Created by byter on 29.07.17.
//

#include "Mesh.h"
#include <threepp/math/Matrix4.h>
#include <threepp/math/Sphere.h>
#include <threepp/math/Triangle.h>
#include <threepp/core/Raycaster.h>

namespace three {

void Mesh::raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const
{
  if (materialCount() == 0) return;

  // Checking boundingSphere distance to ray
  if (geometry()->boundingSphere().isEmpty()) geometry()->computeBoundingSphere();

  math::Sphere sphere = geometry()->boundingSphere();
  sphere.apply(_matrixWorld);

  if (!raycaster.ray().intersectsSphere(sphere)) return;

  math::Matrix4 inverseMatrix = _matrixWorld.inverted();
  math::Ray ray(raycaster.ray());
  ray.apply(inverseMatrix);

  // Check boundingBox before continuing
  if (!geometry()->boundingBox().isEmpty()) {
    if (!ray.intersectsBox(geometry()->boundingBox())) return;
  }

  geometry()->raycast(*this, raycaster, ray, intersects);
}

}