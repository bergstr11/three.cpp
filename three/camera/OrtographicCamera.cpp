//
// Created by byter on 09.09.17.
//

#include "OrtographicCamera.h"

namespace three {

void OrtographicCamera::applyTo(math::Ray &ray, const math::Vector3 &coords)
{
  ray.origin().set( coords.x(), coords.y(), (_near + _far) / (_near - _far) ).unproject(*this); // set origin in plane of camera
  ray.direction().set( 0, 0, - 1 ).transformDirection( _matrixWorld );
}

}