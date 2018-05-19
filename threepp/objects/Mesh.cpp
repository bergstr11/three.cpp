//
// Created by byter on 29.07.17.
//

#include "Mesh.h"
#include <threepp/math/Matrix4.h>
#include <threepp/math/Sphere.h>
#include <threepp/math/Triangle.h>
#include <threepp/core/Raycaster.h>

namespace three {

void Mesh::raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects)
{
  if (materialCount() == 0) return;

  // Checking boundingSphere distance to ray
  if (geometry()->boundingSphere().isEmpty()) geometry()->computeBoundingSphere();

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

  hit = geometry()->boundingBox().isEmpty();
  for(auto &ray : rays) {
    ray.apply(inverseMatrix);

    // Check boundingBox before continuing
    hit = hit || ray.intersectsBox(geometry()->boundingBox());
  }

  if(hit) geometry()->raycast(*this, raycaster, rays, intersects);
}

}