//
// Created by byter on 12/24/17.
//

#include "Orbit.h"
#include <threepp/math/Math.h>
#include <threepp/math/Line3.h>
#include <threepp/camera/PerspectiveCamera.h>
#include <threepp/camera/OrthographicCamera.h>
#include <QDebug>
#include <threepp/util/impl/utils.h>

namespace three {
namespace control {

using namespace std;
using namespace math;

struct Orbit::Impl {
  Orbit &o;

  Impl(Orbit &o) : o(o) {}
  virtual ~Impl() = default;

  virtual void pan(float deltaX, float deltaY) = 0;
  virtual void dollyIn(float dollyScale) = 0;
  virtual void dollyOut(float dollyScale) = 0;
  virtual void showAll(Object3D::Ptr object) = 0;
};

struct Orbit::PerspectiveImpl : public Orbit::Impl {
  PerspectiveCamera *_camera;

  PerspectiveImpl(Orbit &o, PerspectiveCamera *camera) : Impl(o), _camera(camera) {}

  void showAll(Object3D::Ptr object) override
  {
    _camera->updateMatrixWorld( true );

    Vector3 basisX = Vector3::fromMatrixColumn(_camera->matrixWorld(), 0);
    Vector3 basisY = Vector3::fromMatrixColumn(_camera->matrixWorld(), 1);
    Vector3 basisZ = Vector3::fromMatrixColumn(_camera->matrixWorld(), 2);

    float vFov = float(degToRad( _camera->fov() ));
    float verticalTanFov = 2 * tan( vFov / 2 );

    float hFov = 2 * atan( verticalTanFov / 2 * _camera->aspect() );
    float horizontalTanFov = 2 * tan( hFov / 2 );

    Vector3 v;

    v = basisZ * -cos( vFov / 2 );
    Vector3 projectionTop = v;
    Vector3 projectionBottom = v;

    v = basisY  * sin( vFov / 2 );
    projectionTop += v;
    projectionBottom += v.negate();

    v = basisZ * -cos( hFov / 2 );
    Vector3 projectionLeft = v;
    Vector3 projectionRight = v;

    v = basisX * sin( hFov / 2 );
    projectionRight += v;
    projectionLeft += v.negate();

    const auto bbox = object->computeBoundingBox();
    const auto center = bbox.getCenter();
    const auto size = bbox.getSize();

    float distance = std::max(size.y() / verticalTanFov, size.x() / horizontalTanFov);
    v = basisZ  * (distance * 1.0f /*fitRatio*/);

    _camera->position() = center + v;
    auto diff = o.target - center;
    object->translateX(diff.x());
    object->translateY(diff.y());

    o.update();
  }

  void pan(float deltaX, float deltaY) override 
  {
    // perspective
    math::Vector3 offset = _camera->position() - o.target;
    float targetDistance = offset.length();

    // half of the fov is center to top of screen
    targetDistance *= tan((_camera->fov() / 2.0f) * M_PI / 180.0f);

    // we actually don't use screenWidth, since perspective camera is fixed to screen height
    if (deltaX != 0)
      o.panLeft(2.0f * deltaX * targetDistance / o.clientHeight(), _camera->matrix());
    if (deltaY != 0)
      o.panUp(2.0f * deltaY * targetDistance / o.clientHeight(), _camera->matrix());
  }

  void dollyIn(float dollyScale) override
  {
    o.scale /= dollyScale;
    o.update();
  }

  void dollyOut(float dollyScale) override
  {
    o.scale *= dollyScale;
    o.update();
  }
};

struct Orbit::OrthographicImpl : public Orbit::Impl {
  OrthographicCamera *_camera;

  OrthographicImpl(Orbit &o, OrthographicCamera *camera) : Impl(o), _camera(camera) {}

  void showAll(Object3D::Ptr object) override
  {
    qWarning() << "showAll not implemented for OrthographicCamera";
  }

  void pan(float deltaX, float deltaY) override {
    // orthographic
    if (deltaX != 0)
      o.panLeft(deltaX * (_camera->right() - _camera->left()) / _camera->zoom() / o.clientWidth(), _camera->matrix());
    if (deltaY != 0)
      o.panUp(deltaY * (_camera->top() - _camera->bottom()) / _camera->zoom() / o.clientHeight(), _camera->matrix());
  }

  void dollyIn(float dollyScale) override
  {
    _camera->setZoom(std::max(o.minZoom, std::min(o.maxZoom, _camera->zoom() * dollyScale)));
    _camera->updateProjectionMatrix();
    o._zoomChanged = true;
  }

  void dollyOut(float dollyScale) override
  {
    _camera->setZoom(std::max(o.minZoom, std::min(o.maxZoom, _camera->zoom() / dollyScale)));
    _camera->updateProjectionMatrix();
    o._zoomChanged = true;
  }
};

Orbit::Impl *Orbit::makeImpl(Orbit &orbit, Camera::Ptr camera)
{
  if (PerspectiveCamera *pcamera = camera->typer) {
    return new PerspectiveImpl(orbit, pcamera);
  }
  if(OrthographicCamera *ocamera = camera->typer) {
    return new OrthographicImpl(orbit, ocamera);
  }
  return nullptr;
}

Orbit::~Orbit()
{
  if(_impl) delete _impl;
}

Orbit::Orbit(Camera::Ptr camera, const math::Vector3 &target)
: _camera(camera), target(target), _target0(target), _position0(camera->position()), _zoom0(camera->zoom())
{
  _impl = makeImpl(*this, camera);
  if(!_impl) {
    enablePan = false;
    enableZoom = false;
  }
  _quat = math::Quaternion::fromUnitVectors(camera->up(), math::Vector3(0, 1, 0));
  _quatInverse = _quat.inverse();
}

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
  if(_impl) _impl->pan(deltaX, deltaY);
}

void Orbit::_dollyIn(float dollyScale)
{
  if(_impl) _impl->dollyIn(dollyScale);
}

void Orbit::_dollyOut(float dollyScale)
{
  if(_impl) _impl->dollyOut(dollyScale);
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

Vector2 toNDC(const Vector3 &projected, float w, float h)
{
  return Vector2(std::round((projected.x() + 1) * w / 2), std::round(-(projected.y() + 1) * h / 2));
}

void Orbit::showAll(Object3D::Ptr object)
{
  if(_impl) _impl->showAll(object);
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
  return false;
}

void Orbit::doRotate(float x, float y)
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

void Orbit::rotate(float deltaX, float deltaY)
{
  doRotate(_rotateStart.x() + deltaX, _rotateStart.y() + deltaY);
}

void Orbit::pan(float deltaX, float deltaY)
{
  _pan(deltaX, deltaY);
  update();
}

float Orbit::getDistance() const
{
  return _camera ? _camera->position().distanceTo(target) : 0.0f;
}

void Orbit::set(float polar, float azimuth)
{
  //initialize spherical as in update()
  _offset = _camera->position() - target;
  _offset.apply(_quat);
  _spherical.setFromVector3(_offset);

  //set the new angles
  _spherical.phi() = polar;
  _spherical.theta() = azimuth;

  _spherical.makeSafe();

  _offset = math::Vector3::fromSpherical(_spherical);

  //rotate offset back to "camera-up-vector-is-up" space
  _offset.apply(_quatInverse);

  _camera->position() = target + _offset;
  _camera->lookAt(target);
}

void Orbit::reset()
{
  target = _target0;
  _camera->position() = _position0;
  _camera->setZoom(_zoom0);
  _camera->updateProjectionMatrix();

  onChanged.emitSignal(_state);

  update();

  _state = State::NONE;
}

void Orbit::saveState()
{
  _target0 = target;
  _position0 = _camera->position();
  _zoom0 = _camera->zoom();
}

}
}
