//
// Created by byter on 12/25/17.
//

#ifndef THREEPPQ_ORBITCONTROLLERITEM_H
#define THREEPPQ_ORBITCONTROLLERITEM_H

#include <QObject>
#include <threepp/quick/cameras/CameraController.h>
#include "OrbitControls.h"

namespace three {
namespace quick {

class Camera;

class OrbitController : public CameraController, public Interactor
{
public:
  enum Movement {Pan, Rotate, Zoom};

private:
  Q_OBJECT
  Q_PROPERTY(three::quick::Camera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
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

  Camera *_camera = nullptr;

  float _minDistance=0, _maxDistance=std::numeric_limits<float>::infinity();
  float _maxPolarAngle = (float) M_PI;
  bool _enablePan = true, _enableRotate = true, _moving=false;

  Qt::CursorShape _rotateCursor=Qt::ArrowCursor, _panCursor=Qt::DragMoveCursor, _zoomCursor=Qt::SizeBDiagCursor;

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

  void _setEnabled(bool enabled) override
  {
    Interactor::_enabled = enabled;
  }

  float distance() {
    return _controls ? _controls->getDistance() : 0.0f;
  }

  void createControls()
  {
    if(!_camera->camera()) return;

    const auto &tg = _camera->target();
    _controls = OrbitControls::make(_item, _camera->camera(), math::Vector3(tg.x(), tg.y(), tg.z()));
    _controls->minDistance = _minDistance;
    _controls->maxDistance = _maxDistance;
    _controls->maxPolarAngle = _maxPolarAngle;
    _controls->enablePan = _enablePan;
    _controls->enableRotate = _enableRotate;

    _controls->setPanCursor(_panCursor);
    _controls->setZoomCursor(_zoomCursor);
    _controls->setRotateCursor(_rotateCursor);

    _controls->onChanged.connect([this](OrbitControls::State state) {
      emit CameraController::changed();
      emit distanceChanged();

      if(!_moving) {
        switch(state) {
          case OrbitControls::State::PAN:
          case OrbitControls::State::TOUCH_PAN:
            _moving = true;
            emit moveStarted(Movement::Pan);
            break;
          case OrbitControls::State::ROTATE:
          case OrbitControls::State::TOUCH_ROTATE:
            _moving = true;
            emit moveStarted(Movement::Rotate);
            break;
          case OrbitControls::State::DOLLY:
          case OrbitControls::State::TOUCH_DOLLY:
            _moving = true;
            emit moveStarted(Movement::Zoom);
            break;
        }
      }
    });
  }

  Camera *camera() const {return _camera;}

  void setCamera(Camera *camera, ThreeDItem *item) override
  {
    Interactor::setItem(item);
    setCamera(camera);
  }

  void setCamera(Camera *camera)
  {
    if(_camera != camera) {
      _camera = camera;

      createControls();
      QObject::connect(this, &CameraController::changed, _camera, &Camera::updateControllerValues);

      emit cameraChanged();
    }
  }

public:
  explicit OrbitController(QObject *parent = nullptr) : CameraController(parent) {}

  void setItem(ThreeDItem *item) override
  {
    Interactor::setItem(item);
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

  bool handleTouchEvent(QTouchEvent *event) override
  {
    return _controls->handleTouchEvent(event);
  }

  Q_INVOKABLE void rotate(float deltaX, float deltaY) {
    if(_controls) _controls->rotate(deltaX, deltaY);
  }
  Q_INVOKABLE void pan(float deltaX, float deltaY) {
    if(_controls) _controls->pan(deltaX, deltaY);
  }
  Q_INVOKABLE void reset() {
    if(_controls) _controls->reset();
  }
  Q_INVOKABLE void set(const QVector2D spherical) {
    if(_controls) _controls->set(spherical.x(), spherical.y());
  }
  Q_INVOKABLE float polar() {
    return _controls ? _controls->getPolarAngle() : 0.0f;
  }
  Q_INVOKABLE float azimuth() {
    return _controls ? _controls->getAzimuthalAngle() : 0.0f;
  }
  Q_INVOKABLE void zoomIn(float factor) {
    if(_controls) _controls->zoomIn(factor);
  }
  Q_INVOKABLE void zoomOut(float factor) {
    if(_controls) _controls->zoomOut(factor);
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
  void cameraChanged();

  void moveStarted(Movement);
  void moveStopped();
};

}
}
#endif //THREEPPQ_ORBITCONTROLLERITEM_H
