//
// Created by byter on 29.07.17.
//

#include "PerspectiveCamera.h"
#include <threepp/math/Math.h>

namespace three {

using namespace math;

PerspectiveCamera::PerspectiveCamera( float fov, float aspect, float near, float far, const object::Typer &typer)
   : Camera(typer, near, far), _fov(fov), _aspect(aspect)
{
  updateProjectionMatrix();
}

PerspectiveCamera::PerspectiveCamera( float fov, float aspect, float near, float far)
   : Camera(object::Typer(this), near, far), _fov(fov), _aspect(aspect)
{
  updateProjectionMatrix();
}

PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera &camera)
   : Camera(*this, object::Typer(this)), _fov(camera._fov), _aspect(camera._aspect)
{}

void PerspectiveCamera::setFocalLength(float focalLength )
{
  // see http://www.bobatkins.com/photography/technical/field_of_view.html
  auto vExtentSlope = 0.5 * getFilmHeight() / focalLength;

  _fov = (float)(math::RAD2DEG * 2 * std::atan(vExtentSlope));
  updateProjectionMatrix();
}

float PerspectiveCamera::getFocalLength()
{
  auto vExtentSlope = std::tan(math::DEG2RAD * 0.5 * _fov);

  return (float)(0.5 * getFilmHeight() / vExtentSlope);
}

float PerspectiveCamera::getEffectiveFOV()
{
  return (float)(math::RAD2DEG * 2 * std::atan(std::tan(math::DEG2RAD * 0.5 * _fov) / _zoom));
}

unsigned PerspectiveCamera::getFilmWidth()
{
  // film not completely covered in portrait format (aspect < 1)
  return (unsigned)(_filmGauge * std::min(_aspect, 1.0f));
}

unsigned PerspectiveCamera::getFilmHeight()
{
  // film not completely covered in landscape format (aspect > 1)
  return (unsigned)(_filmGauge / std::max(_aspect, 1.0f));
}

void PerspectiveCamera::updateProjectionMatrix()
{
  float top = _near * std::tan((float)math::DEG2RAD * 0.5f * _fov) / _zoom;
  float height = 2.0f * top;
  float width = _aspect * height;
  float left = -0.5f * width;

  if(!_view.isNull()) {
    left += _view.offsetX * width / _view.fullWidth;
    top -= _view.offsetY * height / _view.fullHeight;
    width *= (float)_view.width / _view.fullWidth;
    height *= (float)_view.height / _view.fullHeight;
  }

  if(_filmOffset != 0)
    left += _near * (float)_filmOffset / getFilmWidth();

  _projectionMatrix = math::Matrix4::perspective(left, left + width, top, top - height, _near, _far);
}

math::Ray PerspectiveCamera::ray(float x, float y) const
{
  Vector3 origin = math::Vector3::fromMatrixPosition(_matrixWorld);
  Vector3 direction(x, y, 0.5);
  direction.unproject(*this);
  direction -= origin;
  direction.normalize();

  return Ray(origin, direction);
}

}