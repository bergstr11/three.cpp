//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_SHADERLIB_H
#define THREE_QT_SHADERLIB_H

#include <QString>
#include <QResource>

#include "UniformsLib.h"

namespace three {
namespace gl {

struct ShaderLib
{
  ShaderLib() = delete;

  static void init();

  static Shader cube = {
     {
        tCube(nullptr),
        tFlip(-1),
        opacity(1.0f)
     },
     ":chunk/cube_vert.glsl",
     ":chunk/cube_frag.glsl"
  };
};

}
}
#endif //THREE_QT_SHADERLIB_H
