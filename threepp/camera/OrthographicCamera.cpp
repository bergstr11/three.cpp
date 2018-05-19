//
// Created by byter on 09.09.17.
//

#include "OrthographicCamera.h"

namespace three {

math::Ray OrthographicCamera::ray(float x, float y) const
{
  math::Vector3 origin(x, y, (_near + _far) / (_near - _far));
  origin.unproject(*this);  // set origin in plane of camera
  math::Vector3 direction( 0, 0, - 1 );
  direction.transformDirection( _matrixWorld );

  return math::Ray(origin, direction);
}

void OrthographicCamera::updateProjectionMatrix()
{
  float dx = ( _right - _left ) / ( 2.0f * _zoom );
  float dy = ( _top - _bottom ) / ( 2.0f * _zoom );
  float cx = ( _right + _left ) / 2.0f;
  float cy = ( _top + _bottom ) / 2.0f;

  float left = cx - dx;
  float right = cx + dx;
  float top = cy + dy;
  float bottom = cy - dy;

  if (!_view.isNull()) {

    float zoomW = _zoom / ( (float)_view.width / _view.fullWidth );
    float zoomH = _zoom / ( (float)_view.height / _view.fullHeight );
    float scaleW = ( _right - _left ) / _view.width;
    float scaleH = ( _top - _bottom ) / _view.height;

    left += scaleW * ( (float)_view.offsetX / zoomW );
    right = left + scaleW * ( (float)_view.width / zoomW );
    top -= scaleH * ( (float)_view.offsetY / zoomH );
    bottom = top - scaleH * ( (float)_view.height / zoomH );
  }

  _projectionMatrix = math::Matrix4::orthographic( left, right, top, bottom, _near, _far );
}

}