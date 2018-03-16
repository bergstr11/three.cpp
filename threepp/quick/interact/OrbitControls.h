//
// Created by byter on 12/25/17.
//

#ifndef THREEPPQ_ORBITCONTROLLER_H
#define THREEPPQ_ORBITCONTROLLER_H

#include <QQuickItem>
#include <QCursor>
#include <QMouseEvent>
#include <threepp/controls/Orbit.h>

namespace three {
namespace quick {

class OrbitControls : public control::Orbit
{
  QQuickItem * const _item;
  QCursor _rotateCursor {Qt::ArrowCursor}, _panCursor {Qt::DragMoveCursor}, _zoomCursor {Qt::SizeBDiagCursor};

  QPoint _startingPos;
  Qt::MouseButton _eventButton;

protected:
  explicit OrbitControls(QQuickItem *item, three::Camera::Ptr camera)
     : control::Orbit(camera), _item(item) {}

public:
  using Ptr = std::shared_ptr<OrbitControls>;
  static Ptr make(QQuickItem *item, three::Camera::Ptr camera) {
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
  void setZoomCursor(Qt::CursorShape cursor) {
    _zoomCursor.setShape(cursor);
  }

  void setPanCursor(Qt::CursorShape cursor) {
    _panCursor.setShape(cursor);
  }

  void setRotateCursor(Qt::CursorShape cursor) {
    _rotateCursor.setShape(cursor);
  }

  bool handleMousePressed(QMouseEvent *event)
  {
    if ( enabled  ) {
      _startingPos= event->pos();
      _eventButton= event->button();
    }
    return false;
  }

  bool handleMouseMoved(QMouseEvent *event)
  {
    if(_state == State::NONE) {

      QPoint point = event->pos() - _startingPos;
      if (point.manhattanLength() > 5) {

        switch (_eventButton) {

          case Qt::LeftButton:
            if (!OrbitControls::enableRotate) return false;
            _item->setCursor(_rotateCursor);
            startRotate(event->x(), event->y());
            event->setAccepted(true);
            break;

          case Qt::MidButton:
            if (!OrbitControls::enableZoom) return false;
            _item->setCursor(_zoomCursor);
            startZoom(event->x(), event->y());
            event->setAccepted(true);
            break;

          case Qt::RightButton:
            if (!OrbitControls::enablePan) return false;
            _item->setCursor(_panCursor);
            startPan(event->x(), event->y());
            event->setAccepted(true);
            break;
        }
      }
    }
    else if(handleMove(event->x(), event->y())) {
      event->setAccepted(true);
      return true;
    }
    return false;
  }

  bool handleMouseReleased(QMouseEvent *event)
  {
    _item->unsetCursor();
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

#endif //THREEPPQ_ORBITCONTROLLER_H
