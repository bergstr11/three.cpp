//
// Created by byter on 12/25/17.
//

#ifndef THREEPPQ_ORBITCONTROLLERITEM_H
#define THREEPPQ_ORBITCONTROLLERITEM_H

#include <QObject>
#include <threepp/quick/Three.h>
#include "Controller.h"
#include "OrbitControls.h"

namespace three {
namespace quick {

class OrbitController : public Controller
{
  Q_OBJECT
  Q_PROPERTY(float distance READ distance NOTIFY distanceChanged)
  Q_PROPERTY(float minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(float maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(float maxPolarAngle READ maxPolarAngle WRITE setMaxPolarAngle NOTIFY maxPolarAngleChanged)
  Q_PROPERTY(bool enablePan READ enablePan WRITE setEnablePan NOTIFY enablePanChanged)
  Q_PROPERTY(bool enableRotate READ enableRotate WRITE setEnableRotate NOTIFY enableRotateChanged)

  float _minDistance=0, _maxDistance=std::numeric_limits<float>::infinity();
  float _maxPolarAngle = (float) M_PI;
  bool _enablePan = true, _enableRotate = true;

  OrbitControls::Ptr _controls;

protected:
  float minDistance() const { return _minDistance; }

  void setMinDistance(float minDistance)
  {
    if (_minDistance != minDistance) {
      _minDistance = minDistance;
      emit minDistanceChanged();
    }
  }

  float maxDistance() const { return _maxDistance; }

  void setMaxDistance(float maxDistance)
  {
    if (_maxDistance != maxDistance) {
      _maxDistance = maxDistance;
      emit maxDistanceChanged();
    }
  }

  float maxPolarAngle() const { return _maxPolarAngle; }

  void setMaxPolarAngle(float maxPolarAngle)
  {
    if (_maxPolarAngle != maxPolarAngle) {
      _maxPolarAngle = maxPolarAngle;
      emit maxPolarAngleChanged();
    }
  }

  bool enablePan() const { return _enablePan; }

  void setEnablePan(bool enablePan)
  {
    if (_enablePan != enablePan) {
      _enablePan = enablePan;
      if(_controls) _controls->enablePan = enablePan;
      emit enablePanChanged();
    }
  }

  bool enableRotate() const { return _enableRotate; }

  void setEnableRotate(bool enableRotate)
  {
    if (_enableRotate != enableRotate) {
      _enableRotate = enableRotate;
      if(_controls) _controls->enableRotate = enableRotate;
      emit enableRotateChanged();
    }
  }

  float distance() {
    return _controls ? _controls->getDistance() : 0.0f;
  }

public:
  void setItem(QQuickItem *item) override
  {
    _controls = OrbitControls::make(item, _camera->camera());
    _controls->minDistance = _minDistance;
    _controls->maxDistance = _maxDistance;
    _controls->maxPolarAngle = _maxPolarAngle;
    _controls->enablePan = _enablePan;
    _controls->enableRotate = _enableRotate;

    _controls->onChanged.connect([this]() {
      emit changed();
      emit distanceChanged();
    });
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

  Q_INVOKABLE void reset() {
    if(_controls) _controls->reset();
  }

signals:
  void minDistanceChanged();
  void maxDistanceChanged();
  void maxPolarAngleChanged();
  void enablePanChanged();
  void enableRotateChanged();
  void distanceChanged();
};

}
}
#endif //THREEPPQ_ORBITCONTROLLERITEM_H
