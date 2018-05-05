//
// Created by byter on 1/26/18.
//

#include "OrthographicTrackball.h"
#include <threepp/math/Quaternion.h>

namespace three {
namespace control {

using namespace math;
using namespace std;

Vector2 OrthographicTrackball::getMouseOnScreen(unsigned x, unsigned y)
{
  return Vector2(
     ((float)x - _screen.left) / _screen.width, ((float)y - _screen.top) / _screen.height);
}

Vector3 OrthographicTrackball::getMouseProjectionOnBall(unsigned x, unsigned y)
{
  Vector3 mouseOnBall(
     ((float) x - _screen.width * 0.5f - _screen.left) / radius,
     (_screen.height * 0.5f + _screen.top - y) / radius,
     0.0);

  float length = mouseOnBall.length();

  if (noRoll) {
    if (length < M_SQRT1_2) {
      mouseOnBall.z() = sqrt(1.0f - length * length);
    }
    else {
      mouseOnBall.z() = .5f / length;
    }
  }
  else if (length > 1.0f) {

    mouseOnBall.normalize();
  }
  else {

    mouseOnBall.z() = sqrt(1.0f - length * length);
  }

  _eye = _camera->position() - _target;

  Vector3 vector(_camera->up());
  vector.setLength(mouseOnBall.y());
  vector += Vector3(_camera->up()).cross(_eye).setLength(mouseOnBall.x());
  _eye.setLength(mouseOnBall.z());
  vector += _eye;

  return vector;
}

void OrthographicTrackball::rotateCamera()
{
  float angle = acos(dot(_rotateStart, _rotateEnd) / _rotateStart.length() / _rotateEnd.length());

  if (angle) {
    Vector3 axis = _rotateStart.cross(_rotateEnd).normalize();

    angle *= rotateSpeed;

    Quaternion quaternion(axis, -angle);

    _eye.apply(quaternion);
    _camera->up().apply(quaternion);

    _rotateEnd.apply(quaternion);

    if (staticMoving) {

      _rotateStart = _rotateEnd;
    }
    else {

      quaternion.set(axis, angle * (dynamicDampingFactor - 1.0f));
      _rotateStart.apply(quaternion);
    }
    _changed = true;
  }
}

void OrthographicTrackball::zoomCamera()
{
  if (_state == State::ZOOM_PAN) {
    float factor = _touchZoomDistanceEnd / _touchZoomDistanceStart;
    _touchZoomDistanceStart = _touchZoomDistanceEnd;

    _camera->setZoom(_camera->zoom() * factor);

    _changed = true;
  }
  else {
    float factor = 1.0f + (_zoomEnd.y() - _zoomStart.y()) * zoomSpeed;

    if (abs(factor - 1.0f) > EPS && factor > 0.0f) {

      _camera->setZoom(_camera->zoom() / factor);

      if (staticMoving) {

        _zoomStart = _zoomEnd;
      }
      else {

        _zoomStart.y() += (_zoomEnd.y() - _zoomStart.y()) * dynamicDampingFactor;
      }
      _changed = true;
    }
  }
}

void OrthographicTrackball::panCamera()
{
  //objectUp = new THREE.Vector3(),
  //pan = new THREE.Vector3();

  Vector2 mouseChange = _panEnd - _panStart;

  if (mouseChange.lengthSq()) {

    // Scale movement to keep clicked/dragged position under cursor
    float scale_x = (_camera->right() - _camera->left()) / _camera->zoom();
    float scale_y = (_camera->top() - _camera->bottom()) / _camera->zoom();
    mouseChange.x() *= scale_x;
    mouseChange.y() *= scale_y;

    Vector3 pan(_eye);
    pan.cross(_camera->up()).setLength(mouseChange.x());
    pan += Vector3(_camera->up()).setLength(mouseChange.y());

    _camera->position() += pan;
    _target += pan;

    if (staticMoving) {

      _panStart = _panEnd;
    }
    else {

      _panStart += (_panEnd - _panStart) * dynamicDampingFactor;
    }
    _changed = true;
  }
}

void OrthographicTrackball::update()
{
  _eye = _camera->position() - _target;

  if (!noRotate) {

    rotateCamera();
  }

  if ( !noZoom ) {

    zoomCamera();

    if ( _changed ) {

      _camera->updateProjectionMatrix();
    }
  }

  if ( !noPan ) {
    panCamera();
  }

  _camera->position() = _target + _eye;

  _camera->lookAt(_target);

  if ( _changed ) {

    //_this.dispatchEvent( changeEvent );
    _changed = false;
  }
};

bool OrthographicTrackball::reset()
{
  if(_state == State::NONE) return false;

  _state = State::NONE;
  _prevState = State::NONE;

  _target = _target0;
  _camera->position() = _position0;
  _camera->up() = _up0;

  _eye = _camera->position() - _target;

  _camera->set(_left0, _right0, _top0, _bottom0);

  _camera->lookAt( _target );

  //_this.dispatchEvent( changeEvent );

  _changed = false;
  return true;
};

bool OrthographicTrackball::startRotate(unsigned x, unsigned y)
{
  if (noRotate) return false;

  _rotateStart = getMouseProjectionOnBall(x, y);
  _rotateEnd = _rotateStart;
  return true;
}

bool OrthographicTrackball::startZoom(unsigned x, unsigned y)
{
  if ( noZoom ) return false;

  _zoomStart = getMouseOnScreen( x, y);
  _zoomEnd = _zoomStart;
  return true;
}

bool OrthographicTrackball::startPan(unsigned x, unsigned y)
{
  if ( noPan ) return false;

  _panStart = getMouseOnScreen(x, y);
  _panEnd = _panStart;
  return true;
}

bool OrthographicTrackball::handleMove(unsigned x, unsigned y)
{
  switch(_state) {
    case State::ROTATE: {
      _rotateEnd = getMouseProjectionOnBall(x, y);
      return true;
    }
    case State::PAN: {
      _panEnd = getMouseOnScreen(x, y);
      return true;
    }
    case State::ZOOM: {
      _zoomEnd = getMouseOnScreen(x, y);
      return true;
    }
  }
  return false;
}

bool OrthographicTrackball::handleDelta(int delta)
{
  _zoomStart.y() += delta * 0.01;
  return false;
}

}
}