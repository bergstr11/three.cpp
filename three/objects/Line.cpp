//
// Created by byter on 09.09.17.
//
#include "Line.h"

namespace three {

void Line::raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) override
{
  //float precisionSq = raycaster.linePrecision() * raycaster.linePrecision();

  // Checking boundingSphere distance to ray
  if ( _geometry->boundingSphere().isEmpty()) _geometry->computeBoundingSphere();

  math::Sphere sphere = _geometry->boundingSphere();
  sphere.applyMatrix4(_matrixWorld);

  if (!raycaster.ray().intersectsSphere(sphere)) return;

  math::Matrix4 inverseMatrix = _matrixWorld.inverse();
  math::Ray ray = raycaster.ray();
  ray.applyMatrix4( inverseMatrix );

  //unsigned step = _isLineSegments ? 2 : 1;

  _geometry->raycast(*this, raycaster, ray, intersects);
}

}