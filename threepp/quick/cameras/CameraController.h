//
// Created by byter on 5/13/18.
//

#ifndef THREE_PP_CAMERACONTROLLER_H
#define THREE_PP_CAMERACONTROLLER_H

#include <threepp/quick/ThreeQObjectRoot.h>

namespace three {
namespace quick {

class ThreeDItem;

class Camera;

class CameraController : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

protected:
  bool _enabled = true;
  virtual void _setEnabled(bool enabled) {}

public:
  explicit CameraController(QObject *parent) : ThreeQObjectRoot(parent) {}

  virtual void setCamera(Camera *camera, ThreeDItem *item) = 0;

  bool enabled()
  {
    return _enabled;
  }

  void setEnabled(bool enabled)
  {
    if(_enabled !=enabled) {
      _enabled = enabled;
      _setEnabled(enabled);
      emit enabledChanged();
    }
  }

signals:
  void enabledChanged();
  void changed();
};

}
}
#endif //THREE_PP_CAMERACONTROLLER_H
