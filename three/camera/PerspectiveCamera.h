//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_PERSPECTIVECAMERA
#define THREEQT_PERSPECTIVECAMERA

#include "Camera.h"

namespace three {

class PerspectiveCamera : public Camera
{
  float _fov    = 50;
  float _aspect = 1;
  float _zoom   = 1;
  float _near   = 0.1;
  float _far    = 2000;
  float _focus  = 10;
  unsigned int _filmGauge = 35;
  unsigned int _filmOffset = 0;

  struct View {
    bool isNull = true;
    unsigned fullWidth;
    unsigned fullHeight;
    unsigned offsetX;
    unsigned offsetY;
    unsigned width;
    unsigned height;
  } _view;

protected:
  PerspectiveCamera( float fov, float aspect, float near, float far );

public:
  using Ptr = std::shared_ptr<PerspectiveCamera>;
  static Ptr make(float fov, float aspect, float near, float far) {
    return std::shared_ptr<PerspectiveCamera>(new PerspectiveCamera(fov, aspect, near, far));
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

  void updateProjectionMatrix();

  void applyTo(math::Ray &ray, const math::Vector3 &coords) override;
};

}

#endif //THREEQT_PERSPECTIVECAMERA
