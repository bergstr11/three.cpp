//
// Created by byter on 12/25/17.
//

#ifndef THREEPP_ORBITCONTROLLER_H
#define THREEPP_ORBITCONTROLLER_H

#include <QQuickItem>
#include <QMouseEvent>
#include <controls/OrbitControls.h>

namespace three {
namespace quick {

class OrbitControls : public three::OrbitControls
{
  const QQuickItem * const _item;

protected:
  explicit OrbitControls(const QQuickItem *item, three::Camera::Ptr camera)
     : three::OrbitControls(camera), _item(item) {}

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

        _rotateStart.set( event->x(), event->y());

        state = State::ROTATE;

        event->setAccepted(true);
        break;

      case Qt::MidButton:

        if ( !OrbitControls::enableZoom ) return false;

        _dollyStart.set( event->x(), event->y() );

        event->setAccepted(true);
        state = State::DOLLY;

        break;

      case Qt::RightButton:

        if ( !OrbitControls::enablePan ) return false;

        _panStart.set( event->x(), event->y() );

        state = State::PAN;

        event->setAccepted(true);
        break;
    }
    return false;
  }

  bool handleMouseMoved(QMouseEvent *event)
  {
    switch(state) {
      case State::ROTATE:
        handleMouseMoveRotate(event->x(), event->y());
        event->setAccepted(true);
        return true;
      case State::DOLLY:
        handleMouseMoveDolly(event->x(), event->y());
        event->setAccepted(true);
        return true;
      case State::PAN:
        handleMouseMovePan(event->x(), event->y());
        event->setAccepted(true);
        return true;
      default:
        return false;
    }
  }

  bool handleMouseReleased(QMouseEvent *event)
  {
    if(state != State::NONE) {
      state = State::NONE;
      event->setAccepted(true);
    }
    return false;
  }

  bool handleMouseWheel(QWheelEvent *event)
  {
    if ( event->delta() < 0 ) {
      dollyOut( getZoomScale() );
      event->setAccepted(true);
      return true;
    }
    else if ( event->delta() > 0 ) {
      dollyIn( getZoomScale() );
      event->setAccepted(true);
      return true;
    }

    return false;
  }
};

}
}

#endif //THREEPP_ORBITCONTROLLER_H
