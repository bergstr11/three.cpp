//
// Created by byter on 29.07.17.
//

#include "PerspectiveCamera.h"
#include "math/Math.h"

namespace three {

PerspectiveCamera::PerspectiveCamera( float fov, float aspect, float near, float far )
   : _fov(fov), _aspect(aspect), _near(near), _far(far)
{
  updateProjectionMatrix();
}

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

void PerspectiveCamera::setViewOffset(unsigned fullWidth, unsigned fullHeight, unsigned x, unsigned y, unsigned width, unsigned height)
{
  _aspect = fullWidth / fullHeight;

  _view.fullWidth = fullWidth;
  _view.fullHeight = fullHeight;
  _view.offsetX = x;
  _view.offsetY = y;
  _view.width = width;
  _view.height = height;
  _view.isNull = false;

  updateProjectionMatrix();
}

void PerspectiveCamera::clearViewOffset()
{
  _view.isNull = true;
  updateProjectionMatrix();
}

void PerspectiveCamera::updateProjectionMatrix()
{

  float near = _near;
  double top = near * std::tan(math::DEG2RAD * 0.5 * _fov) / _zoom;
  unsigned height = (unsigned)(2 * top);
  unsigned width = (unsigned)(_aspect * height);
  unsigned left = (unsigned)(-0.5 * width);

  if(!_view.isNull) {
    left += _view.offsetX * width / _view.fullWidth;
    top -= _view.offsetY * height / _view.fullHeight;
    width *= _view.width / _view.fullWidth;
    height *= _view.height / _view.fullHeight;

  }

  if(_filmOffset != 0)
    left += near * _filmOffset / getFilmWidth();

  _projectionMatrix = math::Matrix4::perspective(left, left + width, top, top - height, near, _far);
}

}