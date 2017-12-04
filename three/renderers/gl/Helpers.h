//
// Created by byter on 20.09.17.
//

#ifndef THREE_QT_HELPERS_H
#define THREE_QT_HELPERS_H

#include <string>
#include <Constants.h>
#include <QOpenGLFunctions>

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

struct Buffer
{
  GLuint handle;
  GLenum type;
  unsigned bytesPerElement;
  unsigned version;
};

inline bool check_gl_error(QOpenGLFunctions *f)
{
  GLenum err = f->glGetError();
  if(err != GL_NO_ERROR) {
    char buf[50];
    snprintf(buf, 50, "GL error code: 0x%x", err);
    throw std::logic_error(std::string(buf));
  }
}

}
}
#endif //THREE_QT_HELPERS_H
