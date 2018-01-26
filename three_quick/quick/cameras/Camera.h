//
// Created by byter on 12/12/17.
//

#ifndef THREEPP_QUICK_CAMERA_H
#define THREEPP_QUICK_CAMERA_H

#include <QObject>
#include <QVector3D>
#include <core/Object3D.h>
#include <camera/Camera.h>
#include <quick/objects/ThreeQObject.h>
#include <quick/interact/Controller.h>
#include <quick/Math.h>

namespace three {
namespace quick {

class Scene;

class Camera : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(QVector3D lookAt READ lookAt WRITE setLookAt NOTIFY lookAtChanged)
  Q_PROPERTY(Controller * controller READ controller WRITE setController NOTIFY controllerChanged)

  static const float infinity;

  QVector3D _lookAt {infinity, infinity, infinity};

  Controller *_controller = nullptr;

  three::Camera::Ptr _camera;

protected:
  virtual three::Camera::Ptr _createCamera() {return nullptr;};

  three::Object3D::Ptr _create(Scene *scene) override;

  void _post_create(Scene *scene) override;

public:
  Camera(QObject *parent=nullptr) : ThreeQObject(parent) {}
  Camera(three::Camera::Ptr camera, QObject *parent=nullptr) : ThreeQObject(parent), _camera(camera) {}

  QVector3D lookAt() const {return _lookAt;}

  void setLookAt(QVector3D lookAt) {
    if(_lookAt != lookAt) {
      _lookAt = lookAt;

      if(_camera) {
        updateControllerValues();

        const math::Euler &r = _camera->rotation();
        _rotation = QVector3D(r.x(), r.y(), r.z());
        emit rotationChanged();
      }

      emit lookAtChanged();
    }
  }

  void updateControllerValues()
  {
    const math::Vector3 &p = _camera->position();
    setPosition(QVector3D(p.x(), p.y(), p.z()), false);
    const math::Euler &r = _camera->rotation();
    setRotation(QVector3D(r.x(), r.y(), r.z()), false);
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

  three::Camera::Ptr camera() {return _camera;}

signals:
  void lookAtChanged();
  void controllerChanged();
};

}
}


#endif //THREEPP_QUICK_CAMERA_H
