//
// Created by byter on 12/12/17.
//

#ifndef THREEPP_QUICK_CAMERA_H
#define THREEPP_QUICK_CAMERA_H

#include <QObject>
#include <QVector3D>
#include <core/Object3D.h>
#include <camera/Camera.h>
#include <quick/interact/Controller.h>
#include <quick/Math.h>

namespace three {
namespace quick {

class Camera : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(three::math::Euler rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
  Q_PROPERTY(QVector3D lookAt READ lookAt WRITE setLookAt NOTIFY lookAtChanged)
  Q_PROPERTY(Controller * controller READ controller WRITE setController NOTIFY controllerChanged)

  QVector3D _lookAt {std::numeric_limits<float>::infinity(),
                     std::numeric_limits<float>::infinity(),
                     std::numeric_limits<float>::infinity()};

  QVector3D _position;
  three::math::Euler _rotation;
  Controller *_controller = nullptr;

  three::Camera::Ptr _camera;

protected:
  virtual three::Camera::Ptr _create() = 0;

public:
  QVector3D lookAt() const {return _lookAt;}

  void setLookAt(QVector3D lookAt) {
    if(_lookAt != lookAt) {
      _lookAt = lookAt;
      emit lookAtChanged();
    }
  }

  QVector3D position() {return _position;}

  void setPosition(const QVector3D &position) {
    if(position != _position) {
      _position = position;
      if(_camera) _camera->position() = math::Vector3(_position.x(), _position.y(), _position.z());
      emit positionChanged();
    }
  }

  three::math::Euler rotation() {return _rotation;}

  void setRotation(const three::math::Euler &rotation) {
    if(rotation != _rotation) {
      _rotation = rotation;
      if(_camera) _camera->rotation() = _rotation;
      emit rotationChanged();
    }
  }

  Controller *controller() const {return _controller;}

  void setController(Controller *controller)
  {
    if(_controller != controller) {
      _controller = controller;
      emit controllerChanged();
    }
  }

  virtual void update()
  {
    if(!_lookAt.x() != std::numeric_limits<float>::infinity()) {
      _camera->lookAt(math::Vector3(_lookAt.x(), _lookAt.y(), _lookAt.z()));
      setRotation(_camera->rotation());
    }
  }

  three::Camera::Ptr create(three::Scene::Ptr scene);

  three::Camera::Ptr camera() {return _camera;}

signals:
  void lookAtChanged();
  void positionChanged();
  void controllerChanged();
  void rotationChanged();
};

}
}


#endif //THREEPP_QUICK_CAMERA_H
