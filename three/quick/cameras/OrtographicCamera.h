//
// Created by byter on 1/28/18.
//

#ifndef THREEPP_ORTOGRAPHICCAMERA_H
#define THREEPP_ORTOGRAPHICCAMERA_H

#include "Camera.h"
#include <three/camera/OrtographicCamera.h>

namespace three {
namespace quick {

class OrtographicCamera : public Camera
{
Q_OBJECT
  Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
  Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
  Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)
  Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)

  float _left, _right, _top, _bottom, _near, _far;

  three::OrtographicCamera::Ptr _ortographicCamera;

protected:
  three::Camera::Ptr _createCamera() override
  {
    _ortographicCamera = three::OrtographicCamera::make(_left, _right, _top, _bottom, _near, _far);
    return _ortographicCamera;
  }

public:
  OrtographicCamera(QObject *parent = nullptr) : Camera(parent) {}

  OrtographicCamera(three::OrtographicCamera::Ptr camera, QObject *parent = nullptr)
  : Camera(camera, parent), _ortographicCamera(camera) {}

  float left() const {return _left;}
  float right() const {return _right;}
  float top() const {return _top;}
  float bottom() const {return _bottom;}

  void setLeft(float left) {
    if(_left != left) {
      _left = left;
      emit leftChanged();
    }
  }
  void setRight(float right) {
    if(_right != right) {
      _right = right;
      emit rightChanged();
    }
  }
  void setTop(float top) {
    if(_top != top) {
      _top = top;
      emit topChanged();
    }
  }
  void setBottom(float bottom) {
    if(_bottom != bottom) {
      _bottom = bottom;
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

#endif //THREEPP_ORTOGRAPHICCAMERA_H
