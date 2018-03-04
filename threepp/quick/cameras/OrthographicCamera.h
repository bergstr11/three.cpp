//
// Created by byter on 1/28/18.
//

#ifndef THREEPPQ_ORTOGRAPHICCAMERA_H
#define THREEPPQ_ORTOGRAPHICCAMERA_H

#include "Camera.h"
#include <threepp/camera/OrthographicCamera.h>

namespace three {
namespace quick {

class OrthographicCamera : public Camera
{
Q_OBJECT
  Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
  Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
  Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)
  Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)

  float _left = 0, _right = 0, _top = 0, _bottom = 0;

  three::OrthographicCamera::Ptr _orthographicCamera;

protected:
  three::Camera::Ptr _createCamera(float near, float far) override
  {
    _orthographicCamera = three::OrthographicCamera::make(_left, _right, _top, _bottom, near, far);
    return _orthographicCamera;
  }

public:
  OrthographicCamera(QObject *parent = nullptr) : Camera(parent) {}

  OrthographicCamera(three::OrthographicCamera::Ptr camera, QObject *parent = nullptr)
  : Camera(camera, parent), _orthographicCamera(camera) {}

  float left() const {return _left;}
  float right() const {return _right;}
  float top() const {return _top;}
  float bottom() const {return _bottom;}

  void setLeft(float left) {
    if(_left != left) {
      _left = left;
      if(_orthographicCamera) _orthographicCamera->setLeft(_left);
      emit leftChanged();
    }
  }
  void setRight(float right) {
    if(_right != right) {
      _right = right;
      if(_orthographicCamera) _orthographicCamera->setRight(_right);
      emit rightChanged();
    }
  }
  void setTop(float top) {
    if(_top != top) {
      _top = top;
      if(_orthographicCamera) _orthographicCamera->setTop(_top);
      emit topChanged();
    }
  }
  void setBottom(float bottom) {
    if(_bottom != bottom) {
      _bottom = bottom;
      if(_orthographicCamera) _orthographicCamera->setBottom(_bottom);
      emit bottomChanged();
    }
  }

signals:
  void leftChanged();
  void rightChanged();
  void topChanged();
  void bottomChanged();
};

}
}

#endif //THREEPPQ_ORTOGRAPHICCAMERA_H
