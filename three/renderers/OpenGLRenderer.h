//
// Created by byter on 29.07.17.
//

#ifndef THREE_QT_OPENGLRENDERER
#define THREE_QT_OPENGLRENDERER

#include <QOpenGLContext>
#include <Constants.h>
#include <scene/Scene.h>
#include <camera/Camera.h>
#include "Renderer.h"

namespace three {

struct OpenGLRendererOptions
{
  bool alpha = false;
  bool depth = true;
  bool stencil = true;
  bool antialias = false;
  bool premultipliedAlpha = true;
  bool preserveDrawingBuffer = false;
};

class OpenGLRenderer : public Renderer, private OpenGLRendererOptions
{
protected:
  QOpenGLContext *_context;

  OpenGLRenderer(QOpenGLContext *context, const OpenGLRendererOptions &options=OpenGLRendererOptions())
    : _context(context), OpenGLRendererOptions(options) { }

public:
  using Ptr = std::shared_ptr<OpenGLRenderer>;
  static Ptr make(QOpenGLContext *context, size_t width, size_t height, const OpenGLRendererOptions &options=OpenGLRendererOptions());
  static Target::Ptr makeExternalTarget(GLuint frameBuffer, GLuint texture, size_t width, size_t height, bool depthBuffer=true, bool stencilBuffer=true);
};

}


#endif //THREE_QT_OPENGLRENDERER
