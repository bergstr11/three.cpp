//
// Created by byter on 09.09.17.
//

#ifndef THREE_QT_ORTOGRAPHICCAMERA_H
#define THREE_QT_ORTOGRAPHICCAMERA_H

#include "Camera.h"

namespace three {

class OrtographicCamera : public Camera
{
  float _zoom = 1;
  Viewport _view;

  float _left;
  float _right;
  float _top;
  float _bottom;

  float _near;
  float _far;

protected:
  OrtographicCamera(float left, float right, float top, float bottom, float near, float far)
     : _left(left), _right(right), _top(top), _bottom(bottom), _near(near), _far(far)
     {
       updateProjectionMatrix();
     };

public:
  using Ptr = std::shared_ptr<OrtographicCamera>;
  static Ptr make(float left, float right, float top, float bottom, float near=0.1f, float far=2000.0f) {
    return std::shared_ptr<OrtographicCamera>(new OrtographicCamera(left, right, top, bottom, near, far));
  }

  void applyTo(math::Ray &ray, const math::Vector3 &coords) override;

  void updateProjectionMatrix() override;

  float left() const {return _left;}
  float right() const {return _right;}
  float top() const {return _top;}
  float bottom() const {return _bottom;}
  float near() const {return _near;}
  float far() const {return _far;}
  float zoom() const override {return _zoom;}
  float &zoom() {return _zoom;}
};

}
#endif //THREE_QT_ORTOGRAPHICCAMERA_H
