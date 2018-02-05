//
// Created by byter on 12/12/17.
//

#ifndef THREEPPQ_PERSPECTIVECAMERA_H
#define THREEPPQ_PERSPECTIVECAMERA_H

#include "Camera.h"
#include <threepp/camera/PerspectiveCamera.h>

namespace three {
namespace quick {

class PerspectiveCamera : public Camera
{
Q_OBJECT
  Q_PROPERTY(qreal fov READ fov WRITE setFov NOTIFY fovChanged)
  Q_PROPERTY(qreal aspect READ aspect WRITE setAspect NOTIFY aspectChanged)

  qreal _fov=50, _aspect=1, _near=0.1f, _far=2000;

  three::PerspectiveCamera::Ptr _perspectiveCamera;

protected:
  three::Camera::Ptr _createCamera() override
  {
    _perspectiveCamera = three::PerspectiveCamera::make(_fov, _aspect, _near, _far);
    return _perspectiveCamera;
  }

public:
  PerspectiveCamera(QObject *parent = nullptr) : Camera(parent) {}

  PerspectiveCamera(three::PerspectiveCamera::Ptr camera, QObject *parent = nullptr)
     : Camera(camera, parent), _perspectiveCamera(camera) {}

  qreal fov() const {return _fov;}
  qreal aspect() const {return _aspect;}

  void setFov(qreal fov) {
    if(_fov != fov) {
      _fov = fov;
      emit fovChanged();
    }
  }

  void setAspect(qreal aspect) {
    if(_aspect != aspect) {
      _aspect = aspect;
      if(_perspectiveCamera) {
        _perspectiveCamera->setAspect(_aspect);
      }
      emit aspectChanged();
    }
  }

signals:
  void fovChanged();
  void aspectChanged();
};

}
}


#endif //THREEPPQ_PERSPECTIVECAMERA_H
