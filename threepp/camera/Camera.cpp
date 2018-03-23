//
// Created by byter on 29.07.17.
//

#include "Camera.h"

namespace three {

void Camera::setViewOffset(unsigned fullWidth, unsigned fullHeight,
                                      unsigned x, unsigned y, unsigned width, unsigned height)
{
  setAspect(fullWidth / fullHeight);

  _view.fullWidth = fullWidth;
  _view.fullHeight = fullHeight;
  _view.offsetX = x;
  _view.offsetY = y;
  _view.width = width;
  _view.height = height;

  updateProjectionMatrix();
}

void Camera::clearViewOffset()
{
  _view.clear();
  updateProjectionMatrix();
}

void Camera::clone_setup(Camera &clone)
{
  Object3D::clone_setup(clone);

  clone._matrixWorldInverse = _matrixWorldInverse;
  clone._projectionMatrix = _projectionMatrix;
  clone._zoom = _zoom;
  clone._near = _near;
  clone._far = _far;

  clone._view = _view;
}

}