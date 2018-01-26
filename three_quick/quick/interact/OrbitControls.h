//
// Created by byter on 12/25/17.
//

#ifndef THREEPP_ORBITCONTROLLER_H
#define THREEPP_ORBITCONTROLLER_H

#include <QQuickItem>
#include <QMouseEvent>
#include <controls/Orbit.h>

namespace three {
namespace quick {

class OrbitControls : public control::Orbit
{
  const QQuickItem * const _item;

protected:
  explicit OrbitControls(const QQuickItem *item, three::Camera::Ptr camera)
     : control::Orbit(camera), _item(item) {}

public:
  using Ptr = std::shared_ptr<OrbitControls>;
  static Ptr make(const QQuickItem *item, three::Camera::Ptr camera) {
    return Ptr(new OrbitControls(item, camera));
  }

protected:
  unsigned int clientWidth() override
  {
    return _item->width();
  }

  unsigned int clientHeight() override
  {
    return _item->height();
  }

public:
  bool handleMousePressed(QMouseEvent *event)
  {
    if ( !enabled  ) return false;

    switch ( event->button() ) {

      case Qt::LeftButton:
        if ( !OrbitControls::enableRotate ) return false;
        startRotate(event->x(), event->y());
        event->setAccepted(true);
        break;

      case Qt::MidButton:
        if ( !OrbitControls::enableZoom ) return false;
        startZoom(event->x(), event->y());
        event->setAccepted(true);
        break;

      case Qt::RightButton:
        if ( !OrbitControls::enablePan ) return false;
        startPan(event->x(), event->y());
        event->setAccepted(true);
        break;
    }
    return false;
  }

  bool handleMouseMoved(QMouseEvent *event)
  {
    if(handleMove(event->x(), event->y())) {
      event->setAccepted(true);
      return true;
    }
    return false;
  }

  bool handleMouseReleased(QMouseEvent *event)
  {
    if(resetState()) {
      event->setAccepted(true);
      return true;
    }
    return false;
  }

  bool handleMouseWheel(QWheelEvent *event)
  {
    if(handleDelta(event->delta())) {
      event->setAccepted(true);
      return true;
    }
    return false;
  }
};

}
}

#endif //THREEPP_ORBITCONTROLLER_H
