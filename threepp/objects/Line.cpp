//
// Created by byter on 09.09.17.
//
#include "Line.h"

namespace three {

void Line::raycast(const Raycaster &raycaster, IntersectList &intersects)
{
  // Checking boundingSphere distance to ray
  if ( geometry()->boundingSphere().isEmpty()) geometry()->computeBoundingSphere();

  math::Sphere sphere = geometry()->boundingSphere();
  sphere.apply(_matrixWorld);

  bool hit = false;
  for(const auto &ray : raycaster.rays()) {
    if (ray.intersectsSphere(sphere)) {
      hit = true;
      break;
    }
  }
  if(!hit) return;

  math::Matrix4 inverseMatrix = _matrixWorld.inverted();

  std::vector<math::Ray> rays(raycaster.rays());
  for(auto &ray : rays) ray.apply( inverseMatrix );

  geometry()->raycast(*this, raycaster, rays, intersects);
}

}