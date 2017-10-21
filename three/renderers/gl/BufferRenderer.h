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

class BufferRendererCommon
{
protected:
  DrawMode _mode;
  QOpenGLFunctions *const _fn;
  QOpenGLExtraFunctions *const _fx;
  Extensions &_extensions;
  RenderInfo &_renderInfo;

public:
  BufferRendererCommon(QOpenGLFunctions *fn, QOpenGLExtraFunctions *fnx,
                 Extensions &extensions, RenderInfo &renderinfo)
     : _fn(fn), _fx(fnx), _extensions(extensions), _renderInfo(renderinfo), _mode(DrawMode::Triangles)
  {}

  void setMode(DrawMode mode) {_mode = mode;}
};

class BufferRenderer : private BufferRendererCommon
{
public:
  BufferRenderer(QOpenGLFunctions *fn, QOpenGLExtraFunctions *fnx,
                 Extensions &extensions, RenderInfo &renderinfo)
     : BufferRendererCommon(fn, fnx, extensions, renderinfo)
  {
  }

  void render(GLint start, GLsizei count);
  void renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count);
};

class IndexedBufferRenderer : private BufferRendererCommon
{
  GLenum _type = 0;
  GLsizei _bytesPerElement = 0;

public:
  IndexedBufferRenderer(QOpenGLFunctions *fn, QOpenGLExtraFunctions *fnx,
                 Extensions &extensions, RenderInfo &renderinfo)
     : BufferRendererCommon(fn, fnx, extensions, renderinfo)
  {
  }

  void setIndex(const Buffer &value )
  {
    _type = value.type;
    _bytesPerElement = value.bytesPerElement;
  }

  void render(unsigned start, unsigned count);
  void renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count);
};

}
}
#endif //THREE_QT_BUFFERRENDERER_H
