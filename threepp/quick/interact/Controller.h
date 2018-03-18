//
// Created by byter on 12/25/17.
//

#ifndef THREEQUICK_CONTROLLER_H
#define THREEQUICK_CONTROLLER_H

#include <QObject>
#include <QQuickItem>
#include <QMouseEvent>
#include <threepp/quick/ThreeQObjectRoot.h>

namespace three {
namespace quick {

class Camera;

class Controller : public ThreeQObjectRoot
{
  friend class Camera;
Q_OBJECT
  Q_PROPERTY(three::quick::Camera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
  Q_PROPERTY(bool enabled READ p_enabled WRITE p_setEnabled NOTIFY enabledChanged)

protected:
  ThreeDItem *_item = nullptr;
  Camera *_camera = nullptr;

  virtual bool enabled() = 0;
  virtual void setEnabled(bool enabled) = 0;

  bool p_enabled() {return enabled();}

  void p_setEnabled(bool _enabled) {
    if(_enabled != enabled()) {
      setEnabled(_enabled);
      emit enabledChanged();
    }
  }

public:
  Controller(QObject *parent=nullptr) : ThreeQObjectRoot(parent) {}

  Camera *camera() const {return _camera;}

  void setCamera(three::quick::Camera *camera);

  void setItem(ThreeDItem *item) override;

  virtual bool handleMousePressed(QMouseEvent *event) {return false;}

  virtual bool handleMouseDoubleClicked(QMouseEvent *event) {return false;}

  virtual bool handleMouseMoved(QMouseEvent *event) {return false;}

  virtual bool handleMouseReleased(QMouseEvent *event) {return false;}

  virtual bool handleMouseWheel(QWheelEvent *event) {return false;}

signals:
  void cameraChanged();
  void enabledChanged();
  void changed();
};

}
}

#endif //THREEQUICK_CONTROLLER_H
