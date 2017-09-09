//
// Created by byter on 29.07.17.
//

#include "OpenGLRenderer_impl.h"

namespace three {

OpenGLRenderer::Ptr OpenGLRenderer::make(QOpenGLContext *context, float width, float height, const OpenGLRendererOptions &options)
{
  return std::shared_ptr<OpenGLRenderer>(new OpenGLRenderer_impl(context, width, height));
}

OpenGLRenderer_impl::OpenGLRenderer_impl(QOpenGLContext *context, unsigned width, unsigned height)
   : OpenGLRenderer(context), state(context) {}

/*OpenGLRenderer::OpenGLRenderer(QOpenGLContext *context, OpenGLRendererData data)
   : OpenGLRendererData(data), _context(context)
{
}*/

}