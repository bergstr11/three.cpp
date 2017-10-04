//
// Created by byter on 03.10.17.
//

#ifndef THREE_QT_RENDERTARGETCUBE_H
#define THREE_QT_RENDERTARGETCUBE_H

#include "RenderTarget.h"
namespace three {
namespace gl {

class RenderTargetCube : public RenderTarget
{
  unsigned activeCubeFace = 0; // PX 0, NX 1, PY 2, NY 3, PZ 4, NZ 5
  unsigned activeMipMapLevel = 0;

public:
  RenderTargetCube(float width, float height, const Options &options) : RenderTarget(width, height, options)
  {}

  int bufCount() override {
    return 6;
  }
};

}
}
#endif //THREE_QT_RENDERTARGETCUBE_H
