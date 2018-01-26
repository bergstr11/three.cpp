//
// Created by byter on 1/26/18.
//

#ifndef THREEPP_ORTOGRAPHICTRACKBALLCONTROLS_H
#define THREEPP_ORTOGRAPHICTRACKBALLCONTROLS_H

#include <QQuickItem>
#include <QMouseEvent>
#include <controls/OrtographicTrackball.h>


namespace three {
namespace quick {

class OrtographicTrackballControls : public three::control::OrtographicTrackball
{
  const QQuickItem * const _item;

protected:
  explicit OrtographicTrackballControls(const QQuickItem *item, three::Camera::Ptr camera)
     : control::OrtographicTrackball(camera), _item(item) {}

public:
  using Ptr = std::shared_ptr<OrtographicTrackballControls>;
  static Ptr make(const QQuickItem *item, three::Camera::Ptr camera) {
    return Ptr(new OrtographicTrackballControls(item, camera));
  }

  bool handleMousePressed(QMouseEvent *event)
  {
    if ( !enabled ) return false;

    switch ( event->button() ) {

      case Qt::LeftButton:
        if(startRotate(event->x(), event->y()))
          event->setAccepted(true);
        break;

      case Qt::MidButton:
        if(startZoom(event->x(), event->y()))
          event->setAccepted(true);
        break;

      case Qt::RightButton:
        if(startPan(event->x(), event->y()))
          event->setAccepted(true);
        break;
    }
    return true;
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
    if(reset()) {
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

#endif //THREEPP_ORTOGRAPHICTRACKBALLCONTROLS_H
