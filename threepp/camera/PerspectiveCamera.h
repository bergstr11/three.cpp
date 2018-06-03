//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_PERSPECTIVECAMERA
#define THREEPP_PERSPECTIVECAMERA

#include <threepp/camera/Camera.h>

namespace three {

class DLX PerspectiveCamera : public Camera
{
  float _fov    = 50;
  float _aspect = 1;
  float _focus  = 10;
  unsigned int _filmGauge = 35;
  unsigned int _filmOffset = 0;

protected:
  PerspectiveCamera( float fov, float aspect, float near, float far, const object::Typer &typer);

  PerspectiveCamera( float fov, float aspect, float near, float far);

  PerspectiveCamera(const PerspectiveCamera &camera);

public:
  using Ptr = std::shared_ptr<PerspectiveCamera>;
  static Ptr make(float fov, float aspect, float near, float far) {
    return Ptr(new PerspectiveCamera(fov, aspect, near, far));
  }

  PerspectiveCamera *cloned() const override {
    return new PerspectiveCamera(*this);
  }

  /**
   * Sets the FOV by focal length in respect to the current .filmGauge.
   *
   * The default film gauge is 35, so that the focal length can be specified for
   * a 35mm (full frame) camera.
   *
   * Values for focal length and film gauge must have the same unit.
   */
  void setFocalLength(float focalLength );

  /**
   * Calculates the focal length from the current .fov and .filmGauge.
   */
  float getFocalLength();

  float getEffectiveFOV();

  unsigned getFilmWidth();

  unsigned getFilmHeight();

  void updateProjectionMatrix() override;

  math::Ray ray(float x, float y) const override;

  float fov() const {return _fov;}

  float aspect() const {return _aspect;}

  void setFov(float fov) {
    _fov = fov;
  }

  void setAspect(float aspect) override {
    _aspect = aspect;
    updateProjectionMatrix();
  }

  void setFovAspect(float fov, float aspect) {
    _fov = fov;
    _aspect = aspect;
  }
};

}

#endif //THREEPP_PERSPECTIVECAMERA
