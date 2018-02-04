//
// Created by byter on 1/26/18.
//

#ifndef THREEPP_ORTOGRAPHICTRACKBALL_H
#define THREEPP_ORTOGRAPHICTRACKBALL_H

#include <three/math/Vector2.h>
#include <three/math/Vector3.h>
#include <three/camera/OrtographicCamera.h>

namespace three {
namespace control {

struct Screen {
  unsigned left=0, top=0, width=0, height=0;
};

class OrtographicTrackball
{
protected:
  enum class State : int {NONE=-1, ROTATE=0, ZOOM=1, PAN=2, ZOOM_PAN=3};

  OrtographicCamera::Ptr _camera;
  math::Vector3 _target;

  bool _changed = true;

  Screen _screen;

  State _state = State::NONE;
  State _prevState = State::NONE;

  math::Vector3 _eye;

  math::Vector3 _rotateStart;
  math::Vector3 _rotateEnd;

  math::Vector2 _zoomStart;
  math::Vector2 _zoomEnd;

  float _touchZoomDistanceStart = 0.0f;
  float _touchZoomDistanceEnd = 0.0f;

  math::Vector2 _panStart;
  math::Vector2 _panEnd;

  // for reset
  math::Vector3 _target0;
  math::Vector3 _position0;
  math::Vector3 _up0;

  float _left0;
  float _right0;
  float _top0;
  float _bottom0;

  math::Vector2 getMouseOnScreen(unsigned x, unsigned y);
  math::Vector3 getMouseProjectionOnBall(unsigned x, unsigned y);

  void rotateCamera();
  void zoomCamera();
  void panCamera();
  void update();
  bool reset();

  bool startRotate(unsigned x, unsigned y);
  bool startZoom(unsigned x, unsigned y);
  bool startPan(unsigned x, unsigned y);
  bool handleMove(unsigned x, unsigned y);
  bool handleDelta(int delta);

  OrtographicTrackball(OrtographicCamera::Ptr camera) : _camera(camera)
  {
    _position0 = _camera->position();
    _up0 = _camera->up();

    _left0 = _camera->left();
    _right0 = _camera->right();
    _top0 = _camera->top();
    _bottom0 = _camera->bottom();
  }

public:
  bool enabled = true;

  float radius = 0.0f;

  float rotateSpeed = 1.0f;
  float zoomSpeed = 1.2f;

  bool noRotate = false;
  bool noZoom = false;
  bool noPan = false;
  bool noRoll = false;

  bool staticMoving = false;
  float dynamicDampingFactor = 0.2;

  void setScreen(unsigned left, unsigned top, unsigned width, unsigned height)
  {
    _screen.left = left;
    _screen.top = top;
    _screen.width = width;
    _screen.height = height;
  }
};

}
}

#endif //THREEPP_ORTOGRAPHICTRACKBALL_H
