//
// Created by byter on 12/25/17.
//

#ifndef THREEPP_ORBITCONTROLLERITEM_H
#define THREEPP_ORBITCONTROLLERITEM_H

#include <QObject>
#include <quick/Three.h>
#include "Controller.h"
#include "OrbitControls.h"

namespace three {
namespace quick {

class OrbitController : public Controller
{
  Q_OBJECT
  Q_PROPERTY(int minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(int maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(bool enablePan READ enablePan WRITE setEnablePan NOTIFY enablePanChanged)

  int _minDistance=0, _maxDistance=std::numeric_limits<int>::max();
  bool _enablePan = true;

  OrbitControls::Ptr _controls;

protected:
  int minDistance() const { return _minDistance; }

  void setMinDistance(int minDistance)
  {
    if (_minDistance != minDistance) {
      _minDistance = minDistance;
      emit minDistanceChanged();
    }
  }

  int maxDistance() const { return _maxDistance; }

  void setMaxDistance(int maxDistance)
  {
    if (_maxDistance != maxDistance) {
      _maxDistance = maxDistance;
      emit maxDistanceChanged();
    }
  }

  bool enablePan() const { return _enablePan; }

  void setEnablePan(bool enablePan)
  {
    if (_enablePan != enablePan) {
      _enablePan = enablePan;
      emit enablePanChanged();
    }
  }

public:
  void start(ThreeDItem *item, three::Camera::Ptr camera) override
  {
    _controls = OrbitControls::make(item, camera);
  }

  bool handleMousePressed(QMouseEvent *event) override
  {
    return _controls->handleMousePressed(event);
  }

  bool handleMouseMoved(QMouseEvent *event) override
  {
    return _controls->handleMouseMoved(event);
  }

  bool handleMouseReleased(QMouseEvent *event) override
  {
    return _controls->handleMouseReleased(event);
  }

  bool handleMouseWheel(QWheelEvent *event) override
  {
    return _controls->handleMouseWheel(event);
  }

signals:
  void minDistanceChanged();
  void maxDistanceChanged();
  void enablePanChanged();
};

}
}
#endif //THREEPP_ORBITCONTROLLERITEM_H
