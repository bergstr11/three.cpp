//
// Created by byter on 29.07.17.
//

#include "OpenGLRenderer.h"

namespace three {

OpenGLRenderer::OpenGLRenderer(QOpenGLContext *context, OpenGLRendererData data)
   : OpenGLRendererData(data), _context(context)
{
}

}