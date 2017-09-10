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
  if (_materials.empty()) return;

  // Checking boundingSphere distance to ray
  if (_geometry->boundingSphere().isEmpty()) _geometry->computeBoundingSphere();

  math::Sphere sphere = _geometry->boundingSphere();
  sphere.applyMatrix4(_matrixWorld);

  if (!raycaster.ray().intersectsSphere(sphere)) return;

  math::Matrix4 inverseMatrix = _matrixWorld.inverse();
  math::Ray ray = raycaster.ray() * inverseMatrix;

  // Check boundingBox before continuing
  if (!_geometry->boundingBox().isEmpty()) {
    if (!ray.intersectsBox(_geometry->boundingBox())) return;
  }

  math::Vector3 intersectionPoint;
  math::Vector3 intersectionPointWorld;

  _geometry->raycast(*this, raycaster, ray, intersectionPoint, intersectionPointWorld, intersects);
}

}