//
// Created by byter on 02.10.17.
//

#include "Textures.h"

namespace three {
namespace gl {

void Textures::onTextureDispose(Texture *texture)
{
  texture->onDispose.disconnectAll();

  deallocateTexture( texture );

  _infoMemory.textures --;
}

void Textures::onRenderTargetDispose(RenderTarget *renderTarget)
{
  renderTarget->onDispose.disconnectAll();

  deallocateRenderTarget( renderTarget );

  _infoMemory.textures --;
}

void Textures::deallocateTexture(Texture *texture)
{
  if (!texture->image().isNull() && _properties.has(texture, PropertyKey::__image__webglTextureCube)) {

    // cube texture
    auto textureProperties = _properties.get( texture );
    _fn->glDeleteTextures(1, &textureProperties[PropertyKey::__image__webglTextureCube].gluint_value);

  } else if(_properties.has(texture, PropertyKey::__webglInit)) {

    // 2D texture
    auto textureProperties = _properties.get( texture );
    _fn->glDeleteTextures(1, &textureProperties[PropertyKey::__webglInit].gluint_value);
  }

  // remove all webgl properties
  _properties.remove( texture );
}

void Textures::deallocateRenderTarget(RenderTarget *renderTarget)
{
  if (!renderTarget) return;

  if (_properties.has(renderTarget->texture().get(), PropertyKey::__webglTexture)) {

    auto textureProperties = _properties.get( renderTarget->texture().get() );
    _fn->glDeleteTextures(1, &textureProperties[PropertyKey::__webglTexture].gluint_value);
  }

  if (renderTarget->depthTexture() ) {

    renderTarget->depthTexture()->dispose();
  }

  unsigned bufcount = renderTarget->isWebGLRenderTargetCube ? 6 : 1
  auto renderTargetProperties = _properties.get( renderTarget );

  GLuint *framebuffer = renderTargetProperties[PropertyKey::__webglFramebuffer].gluintp_value;
  _fn->glDeleteFramebuffers(bufcount, framebuffer);

  if( renderTargetProperties.find(PropertyKey::__webglDepthbuffer) != renderTargetProperties.end()) {
    GLuint *depthbuffer = renderTargetProperties[PropertyKey::__webglDepthbuffer].gluintp_value;
    _fn->glDeleteRenderbuffers(bufcount, depthbuffer);
  }

  _properties.remove(renderTarget->texture().get());
  _properties.remove(renderTarget);

}

}
}