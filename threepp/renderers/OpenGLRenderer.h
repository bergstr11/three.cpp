//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_OPENGLRENDERER
#define THREEPP_OPENGLRENDERER

#include <mutex>
#include <QOpenGLContext>
#include <threepp/Constants.h>
#include <threepp/scene/Scene.h>
#include <threepp/camera/Camera.h>
#include "Renderer.h"

namespace three {

struct DLX OpenGLRendererOptions
{
  bool alpha = false;
  bool depth = true;
  bool stencil = true;
  bool antialias = false;
  bool premultipliedAlpha = true;
  bool preserveDrawingBuffer = false;
};

class DLX OpenGLRenderer : public Renderer, public OpenGLRendererOptions
{
protected:
  explicit OpenGLRenderer(const OpenGLRendererOptions &options=OpenGLRendererOptions())
    : OpenGLRendererOptions(options) { }

public:
  // clearing
  bool autoClear = true;
  bool autoClearColor = true;
  bool autoClearDepth = true;
  bool autoClearStencil = true;

  std::mutex mutex;

  using Ptr = std::shared_ptr<OpenGLRenderer>;

  static Ptr make(size_t width, size_t height, float pixelRatio, const OpenGLRendererOptions &options=OpenGLRendererOptions());

  static Target::Ptr makeExternalTarget(GLuint frameBuffer, GLuint texture, size_t width, size_t height,
                                        CullFace faceCulling, FrontFaceDirection faceDirection);

  virtual void initContext() = 0;
  virtual void setShadowMapType(three::ShadowMapType type) = 0;
  virtual void setFaceCulling( CullFace cullFace ) = 0;
  virtual void setFaceDirection(FrontFaceDirection frontFaceDirection ) = 0;
  virtual void clear() = 0;
  virtual void clearDepth() = 0;

  virtual void usePrograms(OpenGLRenderer::Ptr other) = 0;
};

}


#endif //THREEPP_OPENGLRENDERER
