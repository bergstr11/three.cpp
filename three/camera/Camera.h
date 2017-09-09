//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_CAMERA
#define THREEQT_CAMERA

#include <math/Matrix4.h>
#include <math/Vector3.h>
#include <math/Quaternion.h>
#include <core/Object3D.h>

namespace three {

class Camera : public Object3D
{
protected:
  math::Matrix4 _matrixWorldInverse;
  math::Matrix4 _projectionMatrix;

  Camera() : _projectionMatrix(math::Matrix4::identity()), _matrixWorldInverse(_matrixWorld.inverse()) {}

public:

  const math::Matrix4 &projectionMatrix() const {return _projectionMatrix;}
  math::Matrix4 &projectionMatrix() {return _projectionMatrix;}

  math::Vector3 getWorldDirection() const
  {
    math::Quaternion quaternion = getWorldQuaternion();

    return math::Vector3(0, 0, - 1).apply(quaternion);
  }

  void updateMatrixWorld(bool force) {

    Object3D::updateMatrixWorld(force);

    _matrixWorldInverse = _matrixWorld.inverse();
  }
};

}


#endif //THREEQT_CAMERA
