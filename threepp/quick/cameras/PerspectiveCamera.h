//
// Created by byter on 12/12/17.
//

#ifndef THREEPPQ_PERSPECTIVECAMERA_H
#define THREEPPQ_PERSPECTIVECAMERA_H

#include <threepp/quick/cameras/Camera.h>
#include <threepp/camera/PerspectiveCamera.h>

namespace three {
namespace quick {

class PerspectiveCamera : public Camera
{
Q_OBJECT
  Q_PROPERTY(float fov READ fov WRITE setFov NOTIFY fovChanged)
  Q_PROPERTY(float aspect READ aspect WRITE setAspect NOTIFY aspectChanged)

  float _fov=50, _aspect=1;

  three::PerspectiveCamera::Ptr _perspectiveCamera;

protected:
  three::Camera::Ptr _createCamera(float near, float far) override
  {
    _perspectiveCamera = three::PerspectiveCamera::make(_fov, _aspect, near, far);
    return _perspectiveCamera;
  }

public:
  PerspectiveCamera(QObject *parent = nullptr) : Camera(parent) {}

  PerspectiveCamera(float fov, float aspect, float near, float far, QObject *parent = nullptr)
     : Camera(near, far, parent), _fov(fov), _aspect(aspect)
  {}

  PerspectiveCamera(three::PerspectiveCamera::Ptr camera, QObject *parent = nullptr)
     : Camera(camera, parent), _perspectiveCamera(camera), _fov(camera->fov()), _aspect(camera->aspect()) {}

  float fov() const {return _fov;}
  float aspect() const {return _aspect;}

  void setFov(float fov) {
    if(_fov != fov) {
      _fov = fov;
      if(_perspectiveCamera) _perspectiveCamera->setFov(_fov);
      emit fovChanged();
    }
  }

  void setAspect(float aspect) {
    if(_aspect != aspect) {
      _aspect = aspect;
      if(_perspectiveCamera) _perspectiveCamera->setAspect(_aspect);

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
