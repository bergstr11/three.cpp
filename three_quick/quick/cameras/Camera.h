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
  three::math::Euler _rotation {std::numeric_limits<float>::infinity(),
                                std::numeric_limits<float>::infinity(),
                                std::numeric_limits<float>::infinity(),
                                math::Euler::RotationOrder::XYZ};

  Controller *_controller = nullptr;

  three::Camera::Ptr _camera;

protected:
  virtual three::Camera::Ptr _create() = 0;

public:
  QVector3D lookAt() const {return _lookAt;}

  void setLookAt(QVector3D lookAt) {
    if(_lookAt != lookAt) {
      _lookAt = lookAt;

      if(_camera) updateControllerValues();

      emit lookAtChanged();
    }
  }

  QVector3D position() {return _position;}

  void setPosition(const QVector3D &position, bool propagate=true) {
    if(position != _position) {
      _position = position;
      if(_camera && propagate)
        _camera->position().set(_position.x(), _position.y(), _position.z());
      emit positionChanged();
    }
  }

  three::math::Euler rotation() {return _rotation;}

  void setRotation(const three::math::Euler &rotation, bool propagate=true) {
    if(_rotation != rotation) {
      _rotation = rotation;
      if(_camera && propagate) _camera->rotation() = _rotation;
      emit rotationChanged();
    }
  }

  void updateControllerValues()
  {
    const math::Vector3 &p = _camera->position();
    setPosition(QVector3D(p.x(), p.y(), p.z()), false);
    setRotation(_camera->rotation(), false);
  }

  Controller *controller() const {return _controller;}

  void setController(Controller *controller)
  {
    if(_controller != controller) {
      _controller = controller;
      _controller->_camera = this;
      QObject::connect(_controller, &Controller::changed, this, &Camera::updateControllerValues);
      emit controllerChanged();
    }
  }

  QVector3D toQVector3D(const math::Vector3 &vector) {
    return QVector3D(vector.x(), vector.y(), vector.z());
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
