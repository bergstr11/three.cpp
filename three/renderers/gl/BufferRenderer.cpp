//
// Created by byter on 21.10.17.
//

#include "BufferRenderer.h"
#include <core/InterleavedBufferAttribute.h>

namespace three {
namespace gl {

void DefaultBufferRenderer::render(GLint start, GLsizei count)
{
  _fn->glDrawArrays((GLenum)_mode, start, count);
  GLenum err = _fn->glGetError();
  if(err != GL_NO_ERROR) {
    GLenum status = _fn->glCheckFramebufferStatus((GLenum)TextureTarget::twoD);
    std::cout << std::setbase(16) << "error " << err << "; framebuffer status " << status << std::endl;
  }

  _renderInfo.calls ++;
  _renderInfo.vertices += count;

  if (_mode == DrawMode::Triangles) _renderInfo.faces += count / 3;
  else if (_mode == DrawMode::Points) _renderInfo.points += count;
}

void DefaultBufferRenderer::renderInstances(InstancedBufferGeometry::Ptr geometry, GLint start, GLsizei count)
{
  bool extension = _extensions.get(Extension::ANGLE_instanced_arrays);

  if (!extension) {
    throw std::invalid_argument(
       "BufferRenderer: using InstancedBufferGeometry but hardware does not support ANGLE_instanced_arrays");
  }

  BufferAttributeT<float>::Ptr position = geometry->position();

  bufferattribute::Dispatch dispatch;
  dispatch.func<InterleavedBufferAttribute>() = [&](InterleavedBufferAttribute &att) {

    count = att.count();
    _fx->glDrawArraysInstanced((GLenum)_mode, 0, count, geometry->maxInstancedCount());
  };

  if(!position->resolver->getValue(dispatch))
    _fx->glDrawArraysInstanced((GLenum)_mode, start, count, geometry->maxInstancedCount() );

  _renderInfo.calls ++;
  _renderInfo.vertices += count * geometry->maxInstancedCount();

  if (_mode == DrawMode::Triangles) _renderInfo.faces += geometry->maxInstancedCount() * count / 3;
  else if (_mode == DrawMode::Points) _renderInfo.points += geometry->maxInstancedCount() * count;
}

void IndexedBufferRenderer::render(GLint start, GLsizei count)
{
  _fn->glDrawElements((GLenum)_mode, count, _type, (GLvoid *)(start * _bytesPerElement));

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

  _fx->glDrawElementsInstanced((GLenum)_mode, count, _type, (const void *)(start * _bytesPerElement), geometry->maxInstancedCount() );

  _renderInfo.calls ++;
  _renderInfo.vertices += count * geometry->maxInstancedCount();

  if (_mode == DrawMode::Triangles) _renderInfo.faces += geometry->maxInstancedCount() * count / 3;
  else if (_mode == DrawMode::Points) _renderInfo.points += geometry->maxInstancedCount() * count;
}

};
}