//
// Created by byter on 09.09.17.
//
#include "Line.h"

namespace three {

void Line::raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const
{
  // Checking boundingSphere distance to ray
  if ( geometry()->boundingSphere().isEmpty()) geometry()->computeBoundingSphere();

  math::Sphere sphere = geometry()->boundingSphere();
  sphere.apply(_matrixWorld);

  if (!raycaster.ray().intersectsSphere(sphere)) return;

  math::Matrix4 inverseMatrix = _matrixWorld.inverted();
  math::Ray ray = raycaster.ray();
  ray.apply( inverseMatrix );

  geometry()->raycast(*this, raycaster, ray, intersects);
}

}