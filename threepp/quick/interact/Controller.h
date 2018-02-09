//
// Created by byter on 12/25/17.
//

#ifndef THREEQUICK_CONTROLLER_H
#define THREEQUICK_CONTROLLER_H

#include <QObject>
#include <QQuickItem>
#include <QMouseEvent>

namespace three {
namespace quick {

class Camera;

class Controller : public QObject
{
  friend class Camera;
Q_OBJECT

protected:
  Camera *_camera = nullptr;

public:
  virtual void setItem(QQuickItem *item) = 0;

  virtual bool handleMousePressed(QMouseEvent *event) = 0;

  virtual bool handleMouseMoved(QMouseEvent *event) = 0;

  virtual bool handleMouseReleased(QMouseEvent *event) = 0;

  virtual bool handleMouseWheel(QWheelEvent *event) = 0;

signals:
  void changed();
};

}
}

#endif //THREEQUICK_CONTROLLER_H
