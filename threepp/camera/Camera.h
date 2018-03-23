//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_CAMERA
#define THREEPP_CAMERA

#include <threepp/math/Matrix4.h>
#include <threepp/math/Vector3.h>
#include <threepp/math/Quaternion.h>
#include <threepp/math/Ray.h>
#include <threepp/core/Object3D.h>
#include <threepp/util/Resolver.h>

namespace three {

struct Viewport
{
  unsigned fullWidth=0;
  unsigned fullHeight=0;
  unsigned offsetX=0;
  unsigned offsetY=0;
  unsigned width=0;
  unsigned height=0;

  void clear() {
    fullWidth = fullHeight = offsetX = offsetY = width = height = 0;
  }

  bool isNull() {
    return fullWidth == 0 && fullHeight == 0;
  }
};

class Raycaster;

class Camera : public Object3D
{
protected:
  math::Matrix4 _matrixWorldInverse;
  math::Matrix4 _projectionMatrix;
  float _zoom   = 1;
  float _near   = 0.1;
  float _far    = 2000;

  Viewport _view;

  Camera(camera::Resolver::Ptr resolver)
    : Object3D(object::Resolver::makeNull()), cameraResolver(resolver), _projectionMatrix(math::Matrix4::identity()),
      _matrixWorldInverse(_matrixWorld.inverted()) {}

  Camera()
    : Object3D(object::Resolver::makeNull()), cameraResolver(camera::Resolver::makeNull()),
      _projectionMatrix(math::Matrix4::identity()), _matrixWorldInverse(_matrixWorld.inverted()) {}

  virtual ~Camera() {}

  void clone_setup(Camera &cloned);

public:
  using Ptr = std::shared_ptr<Camera>;

  camera::Resolver::Ptr cameraResolver;

  /**
   * Sets an offset in a larger frustum. This is useful for multi-window or
   * multi-monitor/multi-machine setups.
   *
   * For example, if you have 3x2 monitors and each monitor is 1920x1080 and
   * the monitors are in grid like this
   *
   *   +---+---+---+
   *   | A | B | C |
   *   +---+---+---+
   *   | D | E | F |
   *   +---+---+---+
   *
   * then for each monitor you would call it like this
   *
   *   var w = 1920;
   *   var h = 1080;
   *   var fullWidth = w * 3;
   *   var fullHeight = h * 2;
   *
   *   --A--
   *   camera.setOffset( fullWidth, fullHeight, w * 0, h * 0, w, h );
   *   --B--
   *   camera.setOffset( fullWidth, fullHeight, w * 1, h * 0, w, h );
   *   --C--
   *   camera.setOffset( fullWidth, fullHeight, w * 2, h * 0, w, h );
   *   --D--
   *   camera.setOffset( fullWidth, fullHeight, w * 0, h * 1, w, h );
   *   --E--
   *   camera.setOffset( fullWidth, fullHeight, w * 1, h * 1, w, h );
   *   --F--
   *   camera.setOffset( fullWidth, fullHeight, w * 2, h * 1, w, h );
   *
   *   Note there is no reason monitors have to be the same size or in a grid.
   */
  void setViewOffset(unsigned fullWidth, unsigned fullHeight, unsigned x, unsigned y, unsigned width, unsigned height);

  void clearViewOffset();

  const math::Matrix4 &projectionMatrix() const {return _projectionMatrix;}

  const math::Matrix4 &matrixWorldInverse() const {return _matrixWorldInverse;}

  math::Vector3 getWorldDirection() const override
  {
    math::Quaternion quaternion = getWorldQuaternion();

    return math::Vector3(0, 0, - 1).apply(quaternion);
  }

  void updateMatrixWorld(bool force) override {

    Object3D::updateMatrixWorld(force);

    _matrixWorldInverse = _matrixWorld.inverted();
  }

  void lookAt(const math::Vector3 &vector) override
  {
    math::Matrix4 m1( _position, vector, _up );

    _quaternion.set(m1);
  }

  float near() const {return _near;}

  void setNear(float near) {
    _near = near;
  }

  float far() const {return _far;}

  void setFar(float far) {
    _far = far;
  }

  float zoom() const {return _zoom;}

  void setZoom(float zoom) {
    _zoom = zoom;
  }

  void setNearFar(float near, float far) {
    _near = near;
    _far = far;
  }

  virtual void setAspect(float aspect) {}

  virtual void setup(math::Ray &ray, float x, float y) = 0;

  virtual void updateProjectionMatrix() = 0;
};

}


#endif //THREEPP_CAMERA
