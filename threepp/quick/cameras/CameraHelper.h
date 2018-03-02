//
// Created by byter on 3/1/18.
//

#ifndef THREE_PPQ_CAMERAHELPER_H
#define THREE_PPQ_CAMERAHELPER_H

#include <QObject>
#include <threepp/helper/Camera.h>

namespace three {
namespace quick {

class CameraHelper : public QObject
{
Q_OBJECT
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

  bool _visible = true;
  bool _configured = false;

  three::helper::Camera::Ptr _helper;

public:
  bool visible() const {return _visible;}

  void setVisible(bool visible)
  {
    if(_visible != visible) {
      _visible = visible;

      if(_helper) _helper->visible() = _visible;
      emit visibleChanged();
    }
    _configured = true;
  }

  three::helper::Camera::Ptr create(three::Camera::Ptr camera)
  {
    _helper = three::helper::Camera::make("camera_helper", camera);
    return _helper;
  }

  bool configured() {
    return _configured;
  }

  Q_INVOKABLE void update() {

  }

signals:
  void visibleChanged();
};

}
}

#endif //THREE_PPQ_CAMERAHELPER_H
