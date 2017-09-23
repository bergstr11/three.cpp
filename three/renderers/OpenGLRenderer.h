//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_WEBGLRENDERER
#define THREEQT_WEBGLRENDERER

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
  static Ptr make(QOpenGLContext *context, float width, float height, const OpenGLRendererOptions &options=OpenGLRendererOptions());

  OpenGLRenderer &setClearColor(const Color &color) {
    //_backg
  }

  OpenGLRenderer &setSize(unsigned width, unsigned height) {

  }
};

}


#endif //THREEQT_WEBGLRENDERER
