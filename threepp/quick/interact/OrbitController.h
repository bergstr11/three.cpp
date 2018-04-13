//
// Created by byter on 12/25/17.
//

#ifndef THREEPPQ_ORBITCONTROLLERITEM_H
#define THREEPPQ_ORBITCONTROLLERITEM_H

#include <QObject>
#include "Controller.h"
#include "OrbitControls.h"

namespace three {
namespace quick {

class OrbitController : public Controller
{
public:
  enum Movement {Pan, Rotate, Zoom};

private:
  Q_OBJECT
  Q_PROPERTY(float distance READ distance NOTIFY distanceChanged)
  Q_PROPERTY(float minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(float maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(float maxPolarAngle READ maxPolarAngle WRITE setMaxPolarAngle NOTIFY maxPolarAngleChanged)
  Q_PROPERTY(bool enablePan READ enablePan WRITE setEnablePan NOTIFY enablePanChanged)
  Q_PROPERTY(bool enableRotate READ enableRotate WRITE setEnableRotate NOTIFY enableRotateChanged)
  Q_PROPERTY(Qt::CursorShape rotateCursor READ rotateCursor WRITE setRotateCursor NOTIFY rotateCursorChanged)
  Q_PROPERTY(Qt::CursorShape zoomCursor READ zoomCursor WRITE setZoomCursor NOTIFY zoomCursorChanged)
  Q_PROPERTY(Qt::CursorShape panCursor READ panCursor WRITE setPanCursor NOTIFY panCursorChanged)

  Q_ENUM(Movement)

  float _minDistance=0, _maxDistance=std::numeric_limits<float>::infinity();
  float _maxPolarAngle = (float) M_PI;
  bool _enablePan = true, _enableRotate = true, _moving=false, _enabled=true;

  Qt::CursorShape _rotateCursor=Qt::ArrowCursor, _panCursor=Qt::DragMoveCursor, _zoomCursor=Qt::SizeBDiagCursor;

  OrbitControls::Ptr _controls;

protected:
  bool enabled() override
  {
    return _controls ? _controls->enabled : _enabled;
  }

  void setEnabled(bool enabled) override
  {
    _enabled = enabled;
    if(_controls) _controls->enabled = enabled;
  }

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

  Qt::CursorShape zoomCursor() const { return _zoomCursor; }

  void setZoomCursor(Qt::CursorShape cursor)
  {
    if (_zoomCursor != cursor) {
      _zoomCursor = cursor;
      if(_controls) _controls->setZoomCursor(cursor);
      emit zoomCursorChanged();
    }
  }

  Qt::CursorShape panCursor() const { return _panCursor; }

  void setPanCursor(Qt::CursorShape cursor)
  {
    if (_panCursor != cursor) {
      _panCursor = cursor;
      if(_controls) _controls->setPanCursor(cursor);
      emit panCursorChanged();
    }
  }

  Qt::CursorShape rotateCursor() const { return _rotateCursor; }

  void setRotateCursor(Qt::CursorShape cursor)
  {
    if (_rotateCursor != cursor) {
      _rotateCursor = cursor;
      if(_controls) _controls->setRotateCursor(cursor);
      emit rotateCursorChanged();
    }
  }

  float distance() {
    return _controls ? _controls->getDistance() : 0.0f;
  }

  void createControls()
  {
    if(!_camera->camera()) return;

    _controls = OrbitControls::make(_item, _camera->camera());
    _controls->minDistance = _minDistance;
    _controls->maxDistance = _maxDistance;
    _controls->maxPolarAngle = _maxPolarAngle;
    _controls->enablePan = _enablePan;
    _controls->enableRotate = _enableRotate;
    _controls->enabled = _enabled;

    _controls->setPanCursor(_panCursor);
    _controls->setZoomCursor(_zoomCursor);
    _controls->setRotateCursor(_rotateCursor);

    _controls->onChanged.connect([this](OrbitControls::State state) {
      emit changed();
      emit distanceChanged();

      if(!_moving) {
        _moving = true;
        switch(state) {
          case OrbitControls::State::PAN:
          case OrbitControls::State::TOUCH_PAN:
            emit moveStarted(Movement::Pan);
          case OrbitControls::State::ROTATE:
          case OrbitControls::State::TOUCH_ROTATE:
            emit moveStarted(Movement::Rotate);
          case OrbitControls::State::DOLLY:
          case OrbitControls::State::TOUCH_DOLLY:
            emit moveStarted(Movement::Zoom);
        }
      }
    });
  }

public:
  OrbitController(QObject *parent = nullptr) : Controller(parent)
  {
    QObject::connect(this, &Controller::cameraChanged, this, &OrbitController::createControls);
  }

  void setItem(ThreeDItem *item) override
  {
    Controller::setItem(item);
    if(_camera) createControls();
  }

  bool handleMousePressed(QMouseEvent *event) override
  {
    return _controls->handleMousePressed(event);
  }

  bool handleMouseDoubleClicked(QMouseEvent *event) override
  {
    return false;
  }

  bool handleMouseMoved(QMouseEvent *event) override
  {
    return _controls->handleMouseMoved(event);
  }

  bool handleMouseReleased(QMouseEvent *event) override
  {
    if(_moving) {
      _moving = false;
      emit moveStopped();
    }
    return _controls->handleMouseReleased(event);
  }

  bool handleMouseWheel(QWheelEvent *event) override
  {
    return _controls->handleMouseWheel(event);
  }

  Q_INVOKABLE void pan(float deltaX, float deltaY) {
    if(_controls) _controls->pan(deltaX, deltaY);
  }
  Q_INVOKABLE void reset() {
    if(_controls) _controls->reset();
  }
  Q_INVOKABLE void lookAt(const QVector3D &position) {
    if(_controls) _controls->lookAt(math::Vector3(position.x(), position.y(), position.z()));
  }

signals:
  void minDistanceChanged();
  void maxDistanceChanged();
  void maxPolarAngleChanged();
  void enablePanChanged();
  void enableRotateChanged();
  void distanceChanged();
  void rotateCursorChanged();
  void zoomCursorChanged();
  void panCursorChanged();

  void moveStarted(Movement);
  void moveStopped();
};

}
}
#endif //THREEPPQ_ORBITCONTROLLERITEM_H
