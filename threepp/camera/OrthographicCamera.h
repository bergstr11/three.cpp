//
// Created by byter on 09.09.17.
//

#ifndef THREEPP_ORTOGRAPHICCAMERA_H
#define THREEPP_ORTOGRAPHICCAMERA_H

#include "Camera.h"

namespace three {

class OrthographicCamera : public Camera
{
  float _left;
  float _right;
  float _top;
  float _bottom;

protected:
  OrthographicCamera(float left, float right, float top, float bottom, float near, float far)
     : _left(left), _right(right), _top(top), _bottom(bottom)
  {
       _near = near;
       _far = far;
       updateProjectionMatrix();
  }

public:
  using Ptr = std::shared_ptr<OrthographicCamera>;
  static Ptr make(float left, float right, float top, float bottom, float near=0.1f, float far=2000.0f) {
    return std::shared_ptr<OrthographicCamera>(new OrthographicCamera(left, right, top, bottom, near, far));
  }

  void setup(math::Ray &ray, float x, float y) override;

  void updateProjectionMatrix() override;

  void set(float left, float right, float top, float bottom) {
    _left = left; _right = right; _top = top; _bottom = bottom;
    updateProjectionMatrix();
  }

  void set(float left, float right, float top, float bottom, float near, float far) {
    _left = left; _right = right; _top = top; _bottom = bottom; _near = near; _far = far;
    updateProjectionMatrix();
  }

  float left() const {return _left;}
  float right() const {return _right;}
  float top() const {return _top;}
  float bottom() const {return _bottom;}
};

}
#endif //THREEPP_ORTOGRAPHICCAMERA_H
