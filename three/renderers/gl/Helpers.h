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

inline void check_glerror(QOpenGLFunctions *f, const char *file, int line)
{
  GLenum err = f->glGetError();
  if(err != GL_NO_ERROR) {
    std::stringstream ss;
    ss << file << ":" << line;
    do {
      ss << std::setbase(16) << " error code " << err;
    } while((err = glGetError()) != GL_NO_ERROR);

    throw std::logic_error(ss.str());
  }
}

inline void check_framebuffer(QOpenGLFunctions *f, const char *file, int line)
{
  GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(status != GL_FRAMEBUFFER_COMPLETE) {
    std::stringstream ss;
    ss << file << ":" << line;
    switch(status) {
      case GL_FRAMEBUFFER_UNDEFINED:
        ss << "; GL_FRAMEBUFFER_UNDEFINED";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        ss << "; GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        ss << "; GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        ss << "; GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        ss << "; GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        ss << "; GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        ss << "; GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        ss << "; GL_FRAMEBUFFER_UNSUPPORTED";
        break;
      default:
        ss << std::setbase(16) << "; error querying framebuffer status: " << status;
        break;
    }
    throw std::logic_error(ss.str());
  }
}

}
}
#endif //THREE_QT_HELPERS_H
