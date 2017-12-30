//
// Created by byter on 12/25/17.
//

#ifndef THREEPP_CONTROLLER_H
#define THREEPP_CONTROLLER_H

#include <QObject>
#include <camera/Camera.h>

namespace three {
namespace quick {

class ThreeDItem;

class Controller : public QObject
{
Q_OBJECT

public:
  virtual void start(ThreeDItem *item, Camera::Ptr camera) = 0;

  virtual bool handleMousePressed(QMouseEvent *event) = 0;

  virtual bool handleMouseMoved(QMouseEvent *event) = 0;

  virtual bool handleMouseReleased(QMouseEvent *event) = 0;

  virtual bool handleMouseWheel(QWheelEvent *event) = 0;
};

}
}


#endif //THREEPP_CONTROLLER_H
