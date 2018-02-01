//
// Created by byter on 09.09.17.
//

#include "OrtographicCamera.h"

namespace three {

void OrtographicCamera::setup(math::Ray &ray, float x, float y)
{
  ray.origin().set(x, y, (_near + _far) / (_near - _far)).unproject(*this); // set origin in plane of camera
  ray.direction().set( 0, 0, - 1 ).transformDirection( _matrixWorld );
}

void OrtographicCamera::updateProjectionMatrix()
{
  float dx = ( _right - _left ) / ( 2 * _zoom );
  float dy = ( _top - _bottom ) / ( 2 * _zoom );
  float cx = ( _right + _left ) / 2;
  float cy = ( _top + _bottom ) / 2;

  float left = cx - dx;
  float right = cx + dx;
  float top = cy + dy;
  float bottom = cy - dy;

  if (!_view.isNull()) {

    float zoomW = _zoom / ( _view.width / _view.fullWidth );
    float zoomH = _zoom / ( _view.height / _view.fullHeight );
    float scaleW = ( _right - _left ) / _view.width;
    float scaleH = ( _top - _bottom ) / _view.height;

    left += scaleW * ( _view.offsetX / zoomW );
    right = left + scaleW * ( _view.width / zoomW );
    top -= scaleH * ( _view.offsetY / zoomH );
    bottom = top - scaleH * ( _view.height / zoomH );

  }

  _projectionMatrix = math::Matrix4::orthographic( left, right, top, bottom, _near, _far );
}

}