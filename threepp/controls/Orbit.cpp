//
// Created by byter on 12/24/17.
//

#include "Orbit.h"
#include <threepp/math/Math.h>
#include <threepp/camera/PerspectiveCamera.h>
#include <threepp/camera/OrthographicCamera.h>

namespace three {
namespace control {

using namespace std;
using namespace math;

bool Orbit::update()
{
  _offset = _camera->position() - target;

  // rotate offset to "y-axis-is-up" space
  _offset.apply(_quat);

  // angle from z-axis around y-axis
  _spherical.setFromVector3(_offset);

  if (autoRotate && _state == State::NONE) {

    rotateLeft(getAutoRotationAngle());
  }

  _spherical.theta() += _sphericalDelta.theta();
  _spherical.phi() += _sphericalDelta.phi();

  // restrict theta to be between desired limits
  _spherical.theta() = std::max(minAzimuthAngle, std::min(maxAzimuthAngle, _spherical.theta()));

  // restrict phi to be between desired limits
  _spherical.phi() = std::max(minPolarAngle, std::min(maxPolarAngle, _spherical.phi()));

  _spherical.makeSafe();

  _spherical.radius() *= scale;

  // restrict radius to be between desired limits
  _spherical.radius() = std::max(minDistance, std::min(maxDistance, _spherical.radius()));

  // move target to panned location
  target += _panOffset;

  _offset = math::Vector3::fromSpherical(_spherical);

  // rotate offset back to "camera-up-vector-is-up" space
  _offset.apply(_quatInverse);

  _camera->position() = target + _offset;

  _camera->lookAt(target);

  if (enableDamping) {

    _sphericalDelta.theta() *= (1 - dampingFactor);
    _sphericalDelta.phi() *= (1 - dampingFactor);

  }
  else {

    _sphericalDelta.set(0, 0, 0);
  }

  scale = 1;
  _panOffset.set(0, 0, 0);

  // update condition is:
  // min(camera displacement, camera rotation in radians)^2 > EPS
  // using small-angle approximation cos(x/2) = 1 - x^2 / 8

  if (_zoomChanged ||
      lastPosition.distanceToSquared(_camera->position()) > EPS ||
      8 * (1 - lastQuaternion.dot(_camera->quaternion())) > EPS) {

    onChanged.emitSignal(_state);

    lastPosition = _camera->position();
    lastQuaternion = _camera->quaternion();
    _zoomChanged = false;

    return true;
  }

  return false;
}

void Orbit::_pan(float deltaX, float deltaY)
{
  camera::Dispatch dispatch;
  dispatch.func<PerspectiveCamera>() = [&](PerspectiveCamera &camera) {

    // perspective
    math::Vector3 offset = _camera->position() - target;
    float targetDistance = offset.length();

    // half of the fov is center to top of screen
    targetDistance *= tan((camera.fov() / 2.0f) * M_PI / 180.0f);

    // we actually don't use screenWidth, since perspective camera is fixed to screen height
    if (deltaX != 0)
      panLeft(2.0f * deltaX * targetDistance / clientHeight(), camera.matrix());
    if (deltaY != 0)
      panUp(2.0f * deltaY * targetDistance / clientHeight(), camera.matrix());
  };
  dispatch.func<OrthographicCamera>() = [&](OrthographicCamera &camera) {

    // orthographic
    if (deltaX != 0)
      panLeft(deltaX * (camera.right() - camera.left()) / camera.zoom() / clientWidth(), camera.matrix());
    if (deltaY != 0)
      panUp(deltaY * (camera.top() - camera.bottom()) / camera.zoom() / clientHeight(), camera.matrix());
  };

  if (!_camera->cameraResolver->getValue(dispatch)) {
    // camera neither orthographic nor perspective
    enablePan = false;
  }
}

void Orbit::_dollyIn(float dollyScale)
{
  camera::Dispatch dispatch;
  dispatch.func<PerspectiveCamera>() = [&](PerspectiveCamera &camera) {

    scale /= dollyScale;
    update();
  };
  dispatch.func<OrthographicCamera>() = [&](OrthographicCamera &camera) {

    camera.setZoom(std::max(minZoom, std::min(maxZoom, camera.zoom() * dollyScale)));
    camera.updateProjectionMatrix();
    _zoomChanged = true;
  };

  if (!_camera->cameraResolver->getValue(dispatch)) {
    // camera neither orthographic nor perspective
    enableZoom = false;
  }
}

void Orbit::_dollyOut(float dollyScale)
{
  camera::Dispatch dispatch;
  dispatch.func<PerspectiveCamera>() = [&](PerspectiveCamera &camera) {

    scale *= dollyScale;
    update();
  };
  dispatch.func<OrthographicCamera>() = [&](OrthographicCamera &camera) {

    camera.setZoom(std::max(minZoom, std::min(maxZoom, camera.zoom() / dollyScale)));
    camera.updateProjectionMatrix();
    _zoomChanged = true;
  };

  if (!_camera->cameraResolver->getValue(dispatch)) {
    // camera neither orthographic nor perspective
    enableZoom = false;
  }
}

void Orbit::startRotate(unsigned x, unsigned y)
{
  _rotateStart.set(x, y);

  _state = State::ROTATE;
}

void Orbit::startZoom(unsigned x, unsigned y)
{
  _dollyStart.set(x, y);

  _state = State::DOLLY;
}

void Orbit::startPan(unsigned x, unsigned y)
{
  _panStart.set(x, y);

  _state = State::PAN;
}

bool Orbit::resetState()
{
  if (_state != State::NONE) {
    _state = State::NONE;
    return true;
  }
  return false;
}

bool Orbit::handleMove(unsigned x, unsigned y)
{
  switch (_state) {
    case State::ROTATE:
      doRotate(x, y);
      return true;
    case State::DOLLY:
      doDolly(x, y);
      return true;
    case State::PAN:
      doPan(x, y);
      return true;
    default:
      return false;
  }
}

bool Orbit::handleDelta(int delta)
{
  if ( delta < 0 ) {
    _dollyOut(getZoomScale());
    return true;
  }
  else if ( delta > 0 ) {
    _dollyIn(getZoomScale());
    return true;
  }
}

void Orbit::doRotate(unsigned x, unsigned y)
{
  _rotateEnd.set(x, y);
  _rotateDelta = _rotateEnd - _rotateStart;

  // rotating across whole screen goes 360 degrees around
  rotateLeft(2.0f * (float)M_PI * _rotateDelta.x() / clientWidth() * rotateSpeed);

  // rotating up and down along whole screen attempts to go 360, but limited to 180
  rotateUp(2.0f * (float)M_PI * _rotateDelta.y() / clientHeight() * rotateSpeed);

  _rotateStart = _rotateEnd;

  update();
}

void Orbit::doDolly(unsigned x, unsigned y)
{
  _dollyEnd.set(x, y);

  _dollyDelta = _dollyEnd - _dollyStart;

  if (_dollyDelta.y() > 0) {

    _dollyIn(getZoomScale());

  }
  else if (_dollyDelta.y() < 0) {

    _dollyOut(getZoomScale());
  }

  _dollyStart = _dollyEnd;

  update();
}

void Orbit::doPan(unsigned x, unsigned y)
{
  _panEnd.set(x, y);

  _panDelta = _panEnd - _panStart;

  _pan(_panDelta.x(), _panDelta.y());

  _panStart = _panEnd;

  update();
}

void Orbit::pan(unsigned deltaX, unsigned deltaY)
{
  _pan(deltaX, deltaY);
  update();
}

float Orbit::getDistance()
{
  return _camera ? _camera->position().distanceTo(target) : 0.0f;
}

void Orbit::lookAt(const math::Vector3 &position)
{
  math::Vector3 pos(position);
  pos.project(*_camera);

  _offset = _camera->position() - target;

  _offset.apply(_quat);

  _spherical.setFromVector3(_offset);

  _spherical.phi() = pos.inclination();
  _spherical.theta() = pos.azimuth();
  _spherical.makeSafe();

  _offset = math::Vector3::fromSpherical(_spherical);

  _offset.apply(_quatInverse);

  _camera->position() = target + _offset;

  _camera->lookAt(target);
  _camera->updateMatrixWorld(true);
  _camera->updateProjectionMatrix();
}

}
}