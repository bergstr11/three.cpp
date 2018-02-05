//
// Created by byter on 24.10.17.
//

#ifndef THREEPP_ARRAYCAMERA_H
#define THREEPP_ARRAYCAMERA_H

#include "PerspectiveCamera.h"
#include <initializer_list>

namespace three {

class ArrayCamera : public PerspectiveCamera
{
protected:
  ArrayCamera(float fov, float aspect, float near, float far, unsigned cameraCount)
     : PerspectiveCamera(fov, aspect, near, far, camera::ResolverT<ArrayCamera>::make(*this)), cameraCount(cameraCount)
  {
  }

public:
  using Ptr = std::shared_ptr<ArrayCamera>;

  const unsigned cameraCount;

  virtual PerspectiveCamera::Ptr operator[](unsigned index) = 0;
};

template <unsigned _sz>
class ArrayCameraI : public ArrayCamera
{
  std::array<PerspectiveCamera::Ptr, _sz> _cameras;

  ArrayCameraI(float fov, float aspect, float near, float far)
     : ArrayCamera(fov, aspect, near, far, _sz)
  {
    for(unsigned i=0; i<_sz;i++) {
      _cameras[i] = PerspectiveCamera::make(fov, aspect, near, far);
    }
  }

public:
  using Ptr = std::shared_ptr<ArrayCamera>;
  static Ptr make(float fov, float aspect, float near, float far) {
    return Ptr(new ArrayCameraI(fov, aspect, near, far));
  }

  const unsigned cameraCount = _sz;

  PerspectiveCamera::Ptr operator[](unsigned index)
  {
    return _cameras[index];
  }
};

}
#endif //THREEPP_ARRAYCAMERA_H
