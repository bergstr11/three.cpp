//
// Created by byter on 20.10.17.
//

#ifndef THREE_QT_BUFFERRENDERER_H
#define THREE_QT_BUFFERRENDERER_H

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <Constants.h>
#include <core/BufferGeometry.h>
#include "Helpers.h"
#include "Extensions.h"

namespace three {
namespace gl {

class BufferRenderer
{
protected:
  DrawMode _mode;
  QOpenGLFunctions *const _fn;
  QOpenGLExtraFunctions *const _fx;
  Extensions &_extensions;
  RenderInfo &_renderInfo;

  BufferRenderer(QOpenGLFunctions *fn, QOpenGLExtraFunctions *fnx,
                 Extensions &extensions, RenderInfo &renderinfo)
     : _fn(fn), _fx(fnx), _extensions(extensions), _renderInfo(renderinfo), _mode(DrawMode::Triangles)
  {}

public:
  void setMode(DrawMode mode) {_mode = mode;}

  virtual void render(GLint start, GLsizei count) = 0;
  virtual void renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count) = 0;
};

class DefaultBufferRenderer : public BufferRenderer
{
public:
  DefaultBufferRenderer(QOpenGLFunctions *fn, QOpenGLExtraFunctions *fnx,
                 Extensions &extensions, RenderInfo &renderinfo)
     : BufferRenderer(fn, fnx, extensions, renderinfo)
  {
  }

  void render(GLint start, GLsizei count) override;
  void renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count) override;
};

class IndexedBufferRenderer : public BufferRenderer
{
  GLenum _type = 0;
  GLsizei _bytesPerElement = 0;

public:
  IndexedBufferRenderer(QOpenGLFunctions *fn, QOpenGLExtraFunctions *fnx,
                 Extensions &extensions, RenderInfo &renderinfo)
     : BufferRenderer(fn, fnx, extensions, renderinfo)
  {
  }

  void setIndex(const Buffer &value )
  {
    _type = value.type;
    _bytesPerElement = value.bytesPerElement;
  }

  void render(GLint start, GLsizei count) override;
  void renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count) override;
};

}
}
#endif //THREE_QT_BUFFERRENDERER_H
