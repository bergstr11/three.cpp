//
// Created by byter on 21.10.17.
//

#include "BufferRenderer.h"
#include <core/InterleavedBufferAttribute.h>

namespace three {
namespace gl {

void BufferRenderer::render(GLint start, GLsizei count)
{
  _fn->glDrawArrays((GLenum)_mode, start, count);

  _renderInfo.calls ++;
  _renderInfo.vertices += count;

  if (_mode == DrawMode::Triangles) _renderInfo.faces += count / 3;
  else if (_mode == DrawMode::Points) _renderInfo.points += count;
}

void BufferRenderer::renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count)
{
  bool extension = _extensions.get(Extension::ANGLE_instanced_arrays);

  if (!extension) {
    throw std::invalid_argument(
       "BufferRenderer: using InstancedBufferGeometry but hardware does not support ANGLE_instanced_arrays");
  }

  BufferAttributeBase<float>::Ptr position = geometry->position();

  bufferattribute::Functions funcs;
  funcs.interleaved = [&](InterleavedBufferAttribute &att) {

    count = att.count();
    _fx->glDrawArraysInstanced((GLenum)_mode, 0, count, geometry->maxInstancedCount());
  };
  funcs._void = [&] () {
    _fx->glDrawArraysInstanced((GLenum)_mode, start, count, geometry->maxInstancedCount() );
  };

  position->resolver->call(funcs);

  _renderInfo.calls ++;
  _renderInfo.vertices += count * geometry->maxInstancedCount();

  if (_mode == DrawMode::Triangles) _renderInfo.faces += geometry->maxInstancedCount() * count / 3;
  else if (_mode == DrawMode::Points) _renderInfo.points += geometry->maxInstancedCount() * count;
}

void IndexedBufferRenderer::render(unsigned start, unsigned count)
{
  _fn->glDrawElements((GLenum)_mode, count, _type, (GLubyte *)(start * _bytesPerElement));

  _renderInfo.calls ++;
  _renderInfo.vertices += count;

  if (_mode == DrawMode::Triangles) _renderInfo.faces += count / 3;
  else if (_mode == DrawMode::Points) _renderInfo.points += count;

}

void IndexedBufferRenderer::renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count)
{
  bool extension = _extensions.get(Extension::ANGLE_instanced_arrays);

  if (!extension) {
    throw std::invalid_argument(
       "BufferRenderer: using InstancedBufferGeometry but hardware does not support ANGLE_instanced_arrays");
  }

  _fx->glDrawElementsInstanced((GLenum)_mode, count, _type, (GLubyte *)(start * _bytesPerElement), geometry->maxInstancedCount() );

  _renderInfo.calls ++;
  _renderInfo.vertices += count * geometry->maxInstancedCount();

  if (_mode == DrawMode::Triangles) _renderInfo.faces += geometry->maxInstancedCount() * count / 3;
  else if (_mode == DrawMode::Points) _renderInfo.points += geometry->maxInstancedCount() * count;
}

};
}