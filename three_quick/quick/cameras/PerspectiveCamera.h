//
// Created by byter on 12/12/17.
//

#ifndef THREEPP_PERSPECTIVECAMERA_H
#define THREEPP_PERSPECTIVECAMERA_H

#include "Camera.h"
#include <camera/PerspectiveCamera.h>

namespace three {
namespace quick {

class PerspectiveCamera : public Camera
{
Q_OBJECT
  Q_PROPERTY(qreal fov READ fov WRITE setFov NOTIFY fovChanged)
  Q_PROPERTY(qreal aspect READ aspect WRITE setAspect NOTIFY aspectChanged)
  Q_PROPERTY(qreal near READ near WRITE setNear NOTIFY nearChanged)
  Q_PROPERTY(qreal far READ far WRITE setFar NOTIFY farChanged)

  qreal _fov=50, _aspect=1, _near=0.1f, _far=2000;

protected:
  three::Camera::Ptr _create() override {
    return three::PerspectiveCamera::make(_fov, _aspect, _near, _far);
  }

public:
  qreal fov() const {return _fov;}
  qreal aspect() const {return _aspect;}
  qreal near() const {return _near;}
  qreal far() const {return _far;}

  void setFov(qreal fov) {
    if(_fov != fov) {
      _fov = fov;
      emit fovChanged();
    }
  }

  void setAspect(qreal aspect) {
    if(_aspect != aspect) {
      _aspect = aspect;
      emit aspectChanged();
    }
  }

  void setNear(qreal near) {
    if(_near != near) {
      _near = near;
      emit nearChanged();
    }
  }

  void setFar(qreal far) {
    if(_far != far) {
      _far = far;
      emit farChanged();
    }
  }

signals:
  void fovChanged();
  void aspectChanged();
  void nearChanged();
  void farChanged();
};

}
}


#endif //THREEPP_PERSPECTIVECAMERA_H
