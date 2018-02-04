//
// Created by byter on 11.09.17.
//

#ifndef THREE_QT_GLCLIPPING_H
#define THREE_QT_GLCLIPPING_H

#include <three/camera/Camera.h>
#include <three/math/Plane.h>
#include "Properties.h"

namespace three {
namespace gl {

class Clipping
{
  size_t _numGlobalPlanes = 0, _numIntersection = 0, _numPlanes = 0;
  bool _localClippingEnabled = false;
  bool _renderingShadows = false;
  math::Matrix3 _viewNormalMatrix;
  math::Plane _plane;

  std::vector<float> _globalState;
  std::vector<float> _uniformValue;
  bool _uniformChanged = false;

public:

  bool init(std::vector<math::Plane> planes, bool enableLocalClipping, Camera::Ptr camera)
  {
    bool enabled = !planes.empty() || enableLocalClipping ||
                   // enable state of previous frame - the clipping code has to
                   // run another frame in order to reset the state:
                   _numGlobalPlanes != 0 || _localClippingEnabled;

    _localClippingEnabled = enableLocalClipping;

    projectPlanes(planes, camera, 0, false, _globalState);
    _numGlobalPlanes = planes.size();

    return enabled;
  }

  size_t numPlanes() const
  { return _numPlanes; }

  size_t numIntersection() const
  { return _numIntersection; }

  const std::vector<float> &uniformValue() {return _uniformValue;}

  void beginShadows()
  {
    _renderingShadows = true;
    _numPlanes = 0;
  }

  void endShadows()
  {
    _renderingShadows = false;
    resetGlobalState();
  }

  void setState(const std::vector<math::Plane> &planes,
                bool clipIntersection,
                bool clipShadows,
                Camera::Ptr camera,
                MaterialProperties &cache,
                bool fromCache)
  {
    if (!_localClippingEnabled || planes.empty() || _renderingShadows && !clipShadows) {
      // there's no local clipping

      if (_renderingShadows) {
        // there's no global clipping

        _numPlanes = 0;
      }
      else {
        resetGlobalState();
      }
    }
    else {

      size_t nGlobal = _renderingShadows ? 0 : _numGlobalPlanes;
      size_t lGlobal = nGlobal * 4;

      _uniformValue = cache.clippingState; // ensure unique state

      std::vector<float> dstArray;
      projectPlanes(planes, camera, lGlobal, fromCache, cache.clippingState);

      for (size_t i = 0; i < lGlobal; ++i) {
        dstArray[i] = _globalState[i];
      }

      cache.clippingState = dstArray;
      _numIntersection = clipIntersection ? _numPlanes : 0;
      _numPlanes += nGlobal;
    }
  }

  void resetGlobalState()
  {
    if (_uniformValue != _globalState) {
      _uniformValue = _globalState;
      _uniformChanged = _numGlobalPlanes > 0;
    }

    _numPlanes = _numGlobalPlanes;
    _numIntersection = 0;
  }

  bool projectPlanes(std::vector<math::Plane> planes,
                     Camera::Ptr camera,
                     unsigned dstOffset,
                     bool skipTransform,
                     const std::vector<float> &destArray)
  {
    if (!planes.empty()) {

      std::vector<float> &dstArray = _uniformValue;

      if (!skipTransform || dstArray.empty()) {

        size_t flatSize = dstOffset + planes.size() * 4;
        const math::Matrix4 &viewMatrix = camera->matrixWorldInverse();

        _viewNormalMatrix = viewMatrix.normalMatrix();

        if (dstArray.empty() || dstArray.size() < flatSize) {
          dstArray.resize(flatSize);
        }

        for (unsigned i = 0, i4 = dstOffset; i < planes.size(); ++i, i4 += 4) {

          _plane = planes[i];
          _plane.apply(viewMatrix);

          _plane.normal().toArray(dstArray, i4);
          dstArray[i4 + 3] = _plane.constant();
        }
      }

      _uniformValue = dstArray;
      _uniformChanged = true;

      return true;
    }

    _numPlanes = planes.size();

    return false;
  }
};

}
}

#endif //THREE_QT_GLCLIPPING_H
