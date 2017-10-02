//
// Created by byter on 20.09.17.
//

#ifndef THREE_QT_HELPERS_H
#define THREE_QT_HELPERS_H

#include <string>
#include <Constants.h>

namespace three {
namespace gl {

struct MemoryInfo {
  unsigned geomtries = 0;
  unsigned textures = 0;
};

struct RenderInfo
{
  unsigned frame = 0;
  unsigned  calls = 0;
  unsigned  vertices = 0;
  unsigned  faces = 0;
  unsigned  points = 0;
};

struct Parameters {
  bool envMapCubeUV;
  bool bumpMap;
  bool normalMap;
  bool flatShading;
  bool logarithmicDepthBuffer;
  bool envMap;
  unsigned numDirLights;
  unsigned numSpotLights;
  unsigned numRectAreaLights;
  unsigned numPointLights;
  unsigned numHemiLights;

  Precision precision = Precision::highp;
};

}
}
#endif //THREE_QT_HELPERS_H
