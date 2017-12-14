//
// Created by byter on 12/12/17.
//

#ifndef THREEPP_QUICK_CAMERA_H
#define THREEPP_QUICK_CAMERA_H

#include <QObject>
#include <core/Object3D.h>
#include <camera/Camera.h>

namespace three {
namespace quick {

class Camera : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(QObject * lookAt READ lookAt WRITE setLookAt NOTIFY lookAtChanged)

  QObject *_lookAt = nullptr;
  QVector3D _position;

  three::Camera::Ptr _camera;

protected:
  virtual three::Camera::Ptr _create() = 0;

public:
  QObject *lookAt() const {return _lookAt;}

  void setLookAt(QObject * lookAt) {
    if(_lookAt != lookAt) {
      _lookAt = lookAt;
      emit lookAtChanged();
    }
  }

  QVector3D position() {return _position;}

  void setPosition(const QVector3D &position) {
    if(position != _position) {
      _position = position;
      emit positionChanged();
    }
  }

  three::Camera::Ptr create();

signals:
  void lookAtChanged();
  void positionChanged();
};

}
}


#endif //THREEPP_QUICK_CAMERA_H
