//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_SHADERLIB_H
#define THREE_QT_SHADERLIB_H

#include <QString>
#include <QResource>

#include <helper/Shader.h>
#include "UniformsLib.h"

namespace three {
namespace gl {

static void init();

class Shader : public three::Shader
{
  QResource _vertex;
  QByteArray _uncompressedVertex;
  const char *_vertexShader = nullptr;

  QResource _fragment;
  QByteArray _uncompressedFragment;
  const char *_fragmentShader = nullptr;

public:
  Shader(std::string name, const UniformValues &uniforms, const char *vertexPath, const char *fragmentPath);

  const std::string vertexShader() override;

  const std::string fragmentShader() override;
};

namespace shaderlib {

static Shader cube = {
   "CUBE",
   {
      tCube(nullptr),
      tFlip(-1),
      opacity(1.0f)
   },
   ":chunk/cube_vert.glsl",
   ":chunk/cube_frag.glsl"
};

}

}
}
#endif //THREE_QT_SHADERLIB_H
