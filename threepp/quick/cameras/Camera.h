//
// Created by byter on 12/12/17.
//

#ifndef THREEPPQ_CAMERA_H
#define THREEPPQ_CAMERA_H

#include <QObject>
#include <QVector3D>
#include <QQmlListProperty>
#include <threepp/camera/Camera.h>
#include <threepp/quick/lights/Light.h>
#include <threepp/quick/interact/Controller.h>
#include <threepp/quick/Math.h>
#include <threepp/quick/cameras/CameraHelper.h>

namespace three {
namespace quick {

class Scene;

class Camera : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(QVector3D target READ target WRITE setTarget NOTIFY targetChanged)
  Q_PROPERTY(Controller * controller READ controller WRITE setController NOTIFY controllerChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::Light> lights READ lights)
  Q_PROPERTY(CameraHelper *helper READ helper)
  Q_PROPERTY(float near READ near WRITE setNear NOTIFY nearChanged)
  Q_PROPERTY(float far READ far WRITE setFar NOTIFY farChanged)
  Q_CLASSINFO("DefaultProperty", "lights")

  static const float infinity;

  QVector3D _target {infinity, infinity, infinity};

  QList<Light *> _lights;

  Controller *_controller = nullptr;

  CameraHelper _qhelper;

  three::Camera::Ptr _camera;

  static void append_light(QQmlListProperty<Light> *list, Light *light);
  static int count_lights(QQmlListProperty<Light> *);
  static Light *light_at(QQmlListProperty<Light> *, int);
  static void clear_lights(QQmlListProperty<Light> *);

protected:
  virtual three::Camera::Ptr _createCamera(float near, float far) {return nullptr;};

  three::Object3D::Ptr _create(Scene *scene) override;

  void _post_create(Scene *scene) override;

  QQmlListProperty<Light> lights();

public:
  float _near=0.1f, _far=2000;

  Camera(QObject *parent=nullptr) : ThreeQObject(parent) {}
  Camera(three::Camera::Ptr camera, QObject *parent=nullptr)
     : ThreeQObject(camera, parent), _camera(camera) {}

  float near() const {return _near;}
  float far() const {return _far;}

  void setNear(float near) {
    if(_near != near) {
      _near = near;
      if(_camera) _camera->setNear(_near);

      emit nearChanged();
    }
  }

  void setFar(float far) {
    if(_far != far) {
      _far = far;
      if(_camera) _camera->setFar(_far);

      emit farChanged();
    }
  }

  QVector3D target() const {return _target;}

  void setTarget(QVector3D target) {
    if(_target != target) {
      _target = target;

      if(_camera) {
        updateControllerValues();

        const math::Euler &r = _camera->rotation();
        _rotation = QVector3D(r.x(), r.y(), r.z());
        emit rotationChanged();
      }

      emit targetChanged();
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

  CameraHelper *helper() {return &_qhelper;}

  Q_INVOKABLE void updateProjectionMatrix();
  Q_INVOKABLE void updateMatrixWorld();

signals:
  void nearChanged();
  void farChanged();
  void targetChanged();
  void controllerChanged();
};

}
}


#endif //THREEPPQ_CAMERA_H
