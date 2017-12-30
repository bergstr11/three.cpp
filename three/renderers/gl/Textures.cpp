//
// Created by byter on 02.10.17.
//

#include "Textures.h"
#include <textures/DataTexture.h>
#include <textures/ImageTexture.h>
#include <helper/Resolver.h>

namespace three {
namespace gl {

bool needsGenerateMipmaps(const Texture &texture)
{
  return texture.generateMipmaps() && texture.isPowerOfTwo() && texture.minFilter != TextureFilter::Nearest
         && texture.minFilter != TextureFilter ::Linear;
}

void Textures::onRenderTargetDispose(RenderTargetInternal &renderTarget)
{
  deallocateRenderTarget( renderTarget );

  _infoMemory.textures --;
}

void Textures::onRenderTargetDispose(RenderTargetCube &renderTarget)
{
  deallocateRenderTarget( renderTarget );

  _infoMemory.textures --;
}

void Textures::onTextureDispose(Texture &texture)
{
  deallocateTexture( texture );

  _infoMemory.textures --;
}

void Textures::deallocateTexture(Texture &texture)
{
  if(_properties.has(texture)) {

    auto &textureProperties = _properties.get( texture );
    if(textureProperties.image_textureCube.isSet()) {
      // cube texture
      _fn->glDeleteTextures(1, &textureProperties.image_textureCube.get());
    }
    else if(textureProperties.webglInit) {
      // 2D texture
      _fn->glDeleteTextures(1, &textureProperties.texture.get());
    }
    else return;

    // remove all webgl properties
    _properties.remove( texture );
  }
}

void Textures::deallocateRenderTarget(RenderTargetInternal &renderTarget)
{
  if (_properties.has(*renderTarget.texture())) {

    auto &textureProperties = _properties.get( *renderTarget.texture() );
    if(textureProperties.texture.isSet())
      _fn->glDeleteTextures(1, &textureProperties.texture.get());
  }

  renderTarget.dispose();

  _fn->glDeleteFramebuffers(1, &renderTarget.frameBuffer);

  _fn->glDeleteRenderbuffers(1, &renderTarget.renderBuffer);

  _properties.remove(*renderTarget.texture());
}

void Textures::deallocateRenderTarget(RenderTargetCube &renderTarget)
{
  if (_properties.has(*renderTarget.texture())) {

    auto &textureProperties = _properties.get( *renderTarget.texture() );
    if(textureProperties.texture.isSet())
      _fn->glDeleteTextures(1, &textureProperties.texture.get());
  }

  renderTarget.dispose();

  _fn->glDeleteFramebuffers(6, renderTarget.frameBuffers.data());

  if(!renderTarget.renderBuffers.empty())
    _fn->glDeleteRenderbuffers(6, renderTarget.renderBuffers.data());

  _properties.remove(*renderTarget.texture());
}

void Textures::setTexture2D(Texture::Ptr texture, unsigned slot)
{
  auto &textureProperties = _properties.get(texture);

  if (texture->version() > 0 && textureProperties.version != texture->version() ) {

    uploadTexture( textureProperties, *texture, slot );
    return;
  }
  _state.activeTexture(GL_TEXTURE0 + slot );
  _state.bindTexture(TextureTarget::twoD, textureProperties.texture);
}

void Textures::setTextureCube(Texture::Ptr texture, unsigned slot)
{
  auto &textureProperties = _properties.get(texture);

  if ( texture->version() > 0 && textureProperties.version != texture->version() ) {

    texture::Dispatch dispatch;

    TextureFormat extFormat = _extensions.extend(texture->format());
    TextureType extType = _extensions.extend(texture->type());

    auto baseFunc = [&](Texture &texture) {

      GLuint webglTextureCube;
      if (!textureProperties.image_textureCube.isSet()) {

        texture.onDispose.connect(this, &Textures::onTextureDispose);

        _fn->glGenTextures(1, &webglTextureCube);
        textureProperties.image_textureCube = webglTextureCube;
        _infoMemory.textures ++;
      }
      else
        webglTextureCube = textureProperties.image_textureCube;


      _state.activeTexture(GL_TEXTURE0 + slot );
      _state.bindTexture(TextureTarget::cubeMap, webglTextureCube);

      setTextureParameters(TextureTarget::cubeMap, texture);
    };
    dispatch.func<DataCubeTexture>() = [&](DataCubeTexture &texture) {

      baseFunc(texture);

      for (unsigned i = 0; i < 6; i ++) {

        const TextureData &data = texture.data(i);

        if(!texture.compressed()) {

          _state.texImage2D(TextureTarget::cubeMapPositiveX+i, 0, extFormat, data.width(), data.height(),
                            extFormat, extType, data.bytes());
        }
        else {
          for ( size_t j = 0, jl = data.mipmaps().size(); j < jl; j ++ ) {

            const Mipmap &mipmap = data.mipmap(j);

            if ( texture.format() != TextureFormat::RGBA && texture.format() != TextureFormat::RGB ) {

              if ( _state.hasCompressedTextureFormat(extFormat) ) {

                _state.compressedTexImage2D(TextureTarget::cubeMapPositiveX + i, j, extFormat, mipmap.width, mipmap.height, mipmap.data );
              }
              else {
                throw std::invalid_argument("setTextureCube: unsupported compressed texture format");
              }
            }
            else {
              _state.texImage2D(TextureTarget::cubeMapPositiveX + i, j, extFormat, mipmap.width, mipmap.height,
                                extFormat, extType, mipmap.data.data() );
            }

          }
        }
      }
    };
    dispatch.func<ImageCubeTexture>() = [&](ImageCubeTexture &texture) {

      baseFunc(texture);

      for (unsigned i = 0; i < CubeTexture::num_faces; i ++ ) {
        QImage cubeImage = clampToMaxSize( texture.image(i), _capabilities.maxCubemapSize, texture.flipY );

        _state.texImage2D(TextureTarget::cubeMapPositiveX+i, 0, extFormat,
                          cubeImage.width(), cubeImage.height(), extFormat, extType, cubeImage);
      }
    };
    texture->resolver->texture::DispatchResolver::getValue(dispatch);

    if ( needsGenerateMipmaps(*texture) ) {

      _fn->glGenerateMipmap( GL_TEXTURE_CUBE_MAP );
    }

    textureProperties.version = texture->version();

    texture->onUpdate.emitSignal( *texture );

  } else {
    _state.activeTexture(GL_TEXTURE0 + slot );
    _state.bindTexture(TextureTarget::cubeMap, textureProperties.image_textureCube);
  }
}

void Textures::setTextureCubeDynamic( Texture::Ptr texture, unsigned slot )
{
  _state.activeTexture(GL_TEXTURE0 + slot);
  _state.bindTexture(TextureTarget::cubeMap, _properties.get( texture ).texture);
}

void Textures::setTextureParameters(TextureTarget textureTarget, Texture &texture)
{
  if ( texture.isPowerOfTwo()) {
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_S, (GLint)texture.wrapS);
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_T, (GLint)texture.wrapT);

    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MAG_FILTER, (GLint)texture.magFilter);
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MIN_FILTER, (GLint)texture.minFilter);
    check_glerror(_fn);
  }
  else {
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    check_glerror(_fn);

    if ( texture.wrapS != TextureWrapping::ClampToEdge || texture.wrapT != TextureWrapping::ClampToEdge) {
      //console.warn( 'THREE.WebGLRenderer: Texture is not power of two. Texture.wrapS and Texture.wrapT should be set to THREE.ClampToEdgeWrapping.', texture );
    }

    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MAG_FILTER, filterFallback( texture.magFilter ) );
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MIN_FILTER, filterFallback( texture.minFilter ) );
    check_glerror(_fn);

    if ( texture.minFilter != TextureFilter::Nearest && texture.minFilter != TextureFilter::Linear) {
      //console.warn( 'THREE.WebGLRenderer: Texture is not power of two. Texture.minFilter should be set to THREE.NearestFilter or THREE.LinearFilter.', texture );
    }
  }

  if (_extensions.get(Extension::EXT_texture_filter_anisotropic)) {

    if ( texture.type() == TextureType::Float && !_extensions.get(Extension::OES_texture_float_linear)) return;
    if ( texture.type() == TextureType::HalfFloat && !_extensions.get(Extension::OES_texture_half_float_linear)) return;

    if ( texture.anisotropy > 1 ||
       (_properties.has(texture) && _properties.get(texture).currentAnisotropy.isSet())) {

      _fn->glTexParameterf((GLenum)textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                           std::min(texture.anisotropy, (float)_capabilities.getMaxAnisotropy()));
      check_glerror(_fn);
      _properties.get(texture).currentAnisotropy = texture.anisotropy;
    }
  }
}

void Textures::uploadTexture(GlProperties &textureProperties, Texture &texture, unsigned slot )
{
  if (!textureProperties.webglInit) {

    textureProperties.webglInit = true;

    texture.onDispose.connect([this](Texture &t) {
      deallocateTexture( t );

      _infoMemory.textures --;
    });

    GLuint tex;
    _fn->glGenTextures(1, &tex);
    textureProperties.texture = tex;

    _infoMemory.textures ++;
  }

  _state.activeTexture( GL_TEXTURE0 + slot );
  _state.bindTexture( TextureTarget::twoD, textureProperties.texture);

  _fn->glPixelStorei(GL_UNPACK_ALIGNMENT, texture.unpackAlignment() );

  setTextureParameters(TextureTarget::twoD, texture );

  texture::Dispatch dispatch;
  dispatch.func<DepthTexture>() = [&](DepthTexture &texture) {

    // populate depth texture with dummy data

    TextureFormat internalFormat = TextureFormat::DepthComponent;

    if (texture.type() == TextureType::Float) {

      internalFormat = TextureFormat::DepthComponent32;

    } else /*WebGL2*/{

      // WebGL 2.0 requires signed internalformat for glTexImage2D
      internalFormat = TextureFormat::DepthComponent16;
    }

    if ( texture.format() == TextureFormat::Depth && internalFormat == TextureFormat::DepthComponent) {

      // The error INVALID_OPERATION is generated by texImage2D if format and internalformat are
      // DEPTH_COMPONENT and type is not UNSIGNED_SHORT or UNSIGNED_INT
      // (https://www.khronos.org/registry/webgl/extensions/WEBGL_depth_texture/)
      if ( texture.type() != TextureType::UnsignedShort && texture.type() != TextureType::UnsignedInt) {

        //console.warn( 'THREE.WebGLRenderer: Use UnsignedShortType or UnsignedIntType for DepthFormat DepthTexture.' );

        texture.type() = TextureType::UnsignedShort;
      }
    }

    // Depth stencil textures need the DEPTH_STENCIL internal format
    // (https://www.khronos.org/registry/webgl/extensions/WEBGL_depth_texture/)
    if ( texture.format() == TextureFormat::DepthStencil) {

      internalFormat = TextureFormat::DepthStencil;

      // The error INVALID_OPERATION is generated by texImage2D if format and internalformat are
      // DEPTH_STENCIL and type is not UNSIGNED_INT_24_8_WEBGL.
      // (https://www.khronos.org/registry/webgl/extensions/WEBGL_depth_texture/)
      if (texture.type() != TextureType::UnsignedInt248 ) {

        //console.warn( 'THREE.WebGLRenderer: Use UnsignedInt248Type for DepthStencilFormat DepthTexture.' );

        texture.type() = TextureType::UnsignedInt248;
      }
    }

    _state.texImage2D(TextureTarget::twoD, 0, internalFormat, texture.width(), texture.height(), texture.format(), texture.type());
  };
  dispatch.func<DataTexture>() = [&](DataTexture &texture) {
    if(texture.compressed()) {
      for ( size_t i = 0, il = texture.mipmaps().size(); i < il; i ++ ) {

        const Mipmap &mipmap = texture.mipmaps()[ i ];

        if ( texture.format() != TextureFormat::RGBA && texture.format() != TextureFormat::RGB) {

          const auto &formats = _state.getCompressedTextureFormats();
          if (std::find(formats.cbegin(), formats.cend(), (GLint)texture.format()) != formats.cend()) {

            _state.compressedTexImage2D(TextureTarget::twoD, i, texture.format(), mipmap.width, mipmap.height, mipmap.data);
          }
          else {
            throw std::invalid_argument("uploadTexture: unsupported compressed texture format");
          }

        } else {
          _state.texImage2D(TextureTarget::twoD, i, texture.format(), mipmap.width, mipmap.height, texture.format(),
                            texture.type(), mipmap.data.data() );
        }
      }
    }
    else {
      // use manually created mipmaps if available
      // if there are no manual mipmaps
      // set 0 level mipmap and then use GL to generate other mipmap levels

      if ( texture.mipmaps().size() > 0 && texture.isPowerOfTwo()) {

        for ( size_t i = 0, il = texture.mipmaps().size(); i < il; i ++ ) {

          const Mipmap &mipmap = texture.mipmaps()[ i ];
          _state.texImage2D(TextureTarget::twoD, i, texture.format(),
                            mipmap.width, mipmap.height, texture.format(), texture.type(), mipmap.data.data() );
        }

        texture.setGenerateMipmaps(false);
      }
      else {
        _state.texImage2D(TextureTarget::twoD, 0, texture.format(),
                          texture.width(), texture.height(), texture.format(), texture.type(), texture.bytes());
      }
    }
  };
  dispatch.func<ImageTexture>() = [&](ImageTexture &texture) {

    // regular Texture (image, video, canvas)
    QImage image = clampToMaxSize( texture.image(), _capabilities.maxTextureSize, false);

    if(texture.premultiplyAlpha()) {
      image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
      _fn->glBlendEquation(GL_FUNC_ADD); //TODO ?
      _fn->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //TODO ?
    }

    if (!(texture.needsPowerOfTwo() && texture.isPowerOfTwo())) {

      image = makePowerOfTwo( image );
    }

    // use manually created mipmaps if available
    // if there are no manual mipmaps
    // set 0 level mipmap and then use GL to generate other mipmap levels
    if ( texture.mipmaps().size() > 0 && texture.isPowerOfTwo()) {

      for ( size_t i = 0, il = texture.mipmaps().size(); i < il; i ++ ) {

        const Mipmap &mipmap = texture.mipmaps()[ i ];
        _state.texImage2D(TextureTarget::twoD, i, texture.format(), texture.format(), texture.type(), mipmap );
      }

      texture.setGenerateMipmaps(false);
    }
    else {
      _state.texImage2D(TextureTarget::twoD, 0, texture.format(), texture.format(), texture.type(), image );
    }
  };
  texture.resolver->texture::DispatchResolver::getValue(dispatch);

  if ( needsGenerateMipmaps(texture) ) _fn->glGenerateMipmap(GL_TEXTURE_2D );

  textureProperties.version = texture.version();

  texture.onUpdate.emitSignal(texture);
}

// Render targets

// Setup storage for target texture and bind it to correct framebuffer
void Textures::setupFrameBufferTexture(GLuint framebuffer, const Renderer::Target &renderTarget, GLenum attachment, TextureTarget textureTarget)
{
  TextureFormat format = renderTarget.texture()->format();
  TextureType type = renderTarget.texture()->type();

  _state.texImage2D( textureTarget, 0, format, renderTarget.width(), renderTarget.height(), format, type, nullptr );
  _fn->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer );
  _fn->glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, (GLenum)textureTarget, _properties.get(renderTarget.texture()).texture, 0 );
  _fn->glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
}

// Setup storage for internal depth/stencil buffers and bind to correct framebuffer
void Textures::setupRenderBufferStorage(GLuint renderbuffer, const RenderTarget &renderTarget )
{
  GLint oldRB;
  _fn->glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRB);

  _fn->glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer );

  if ( renderTarget.depthBuffer() && !renderTarget.stencilBuffer()) {

    _fn->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, renderTarget.width(), renderTarget.height() );
    _fn->glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer );

  } else if ( renderTarget.depthBuffer() && renderTarget.stencilBuffer() ) {

    _fn->glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_STENCIL, renderTarget.width(), renderTarget.height() );
    _fn->glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer );

  } else {
    // FIXME: We don't support !depth !stencil
    _fn->glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA4, renderTarget.width(), renderTarget.height() );
  }

  _fn->glBindRenderbuffer( GL_RENDERBUFFER, oldRB );
}

// Setup resources for a Depth Texture for a FBO (needs an extension)
void Textures::setupDepthTexture(GLuint framebuffer, RenderTargetInternal &renderTarget )
{
  _fn->glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );

  GLuint webglDepthTexture = _properties.get(renderTarget.depthTexture()).texture;

  // upload an empty depth texture with framebuffer size
  if ( webglDepthTexture < 0
       || renderTarget.depthTexture()->width() != renderTarget.width()
       || renderTarget.depthTexture()->height() != renderTarget.height()) {

    renderTarget.depthTexture()->width() = (size_t)renderTarget.width();
    renderTarget.depthTexture()->height() = (size_t)renderTarget.height();
    renderTarget.depthTexture()->needsUpdate(true);
  }

  setTexture2D( renderTarget.depthTexture(), 0 );
  check_glerror(_fn);

  switch(renderTarget.depthTexture()->format()) {
    case TextureFormat::Depth:
      _fn->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, webglDepthTexture, 0);
      break;
    case TextureFormat::DepthStencil:
      _fn->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, webglDepthTexture, 0);
      break;
    default:
      throw std::invalid_argument("unknown depth texture format");
  }
  check_glerror(_fn);
}

// Setup GL resources for a non-texture depth buffer
void Textures::setupDepthRenderbuffer(RenderTargetInternal &renderTarget)
{
  if ( renderTarget.depthTexture() ) {

    setupDepthTexture(renderTarget.frameBuffer, renderTarget);
  }
  else {
    _fn->glBindFramebuffer( GL_FRAMEBUFFER, renderTarget.frameBuffer);
    _fn->glGenRenderbuffers(1, &renderTarget.renderBuffer);
    setupRenderBufferStorage(renderTarget.renderBuffer, renderTarget);
    check_glerror(_fn);
  }

  _fn->glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
}

void Textures::setupDepthRenderbuffer(RenderTargetCube &renderTarget)
{
  renderTarget.renderBuffers.resize(6);

  for ( unsigned i = 0; i < 6; i ++ ) {
    _fn->glBindFramebuffer( GL_FRAMEBUFFER, renderTarget.frameBuffers[ i ] );
    _fn->glGenRenderbuffers(1, &renderTarget.renderBuffers[i]);
    setupRenderBufferStorage( renderTarget.renderBuffers[i], renderTarget );
  }

  _fn->glBindFramebuffer( GL_FRAMEBUFFER, _defaultFBO );
}

// Set up GL resources for the render target
void Textures::setupRenderTarget(RenderTargetInternal &renderTarget)
{
  auto &textureProperties = _properties.get( renderTarget.texture() );

  renderTarget.onDispose.connect(this, &Textures::onRenderTargetDispose);

  GLuint tex;
  _fn->glGenTextures(1, &tex);
  textureProperties.texture = tex;

  _infoMemory.textures ++;

  // Setup framebuffer
  _fn->glGenFramebuffers(1, &renderTarget.frameBuffer);

  // Setup color buffer
  _state.bindTexture( TextureTarget::twoD, textureProperties.texture);
  setTextureParameters(renderTarget.textureTarget, *renderTarget.texture());
  setupFrameBufferTexture( renderTarget.frameBuffer, renderTarget, GL_COLOR_ATTACHMENT0, renderTarget.textureTarget);

  if (needsGenerateMipmaps(*renderTarget.texture())) _fn->glGenerateMipmap((GLenum)renderTarget.textureTarget);
  _state.bindTexture(renderTarget.textureTarget, 0 );

  // Setup depth and stencil buffers
  if ( renderTarget.depthBuffer() ) {

    setupDepthRenderbuffer( renderTarget );
  }
  check_framebuffer(_fn);
}

void Textures::setupRenderTarget(RenderTargetExternal &renderTarget)
{
  _fn->glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.frameBuffer);

  auto &textureProperties = _properties.get( renderTarget.texture() );

  textureProperties.texture = renderTarget.textureHandle();

  //_infoMemory.textures ++;
  _state.currentTextureSlot = 0;
  _state.currentBoundTextures.emplace(_state.currentTextureSlot,
                                      State::BoundTexture(TextureTarget::twoD, textureProperties.texture));

  _state.bindTexture( TextureTarget::twoD, textureProperties.texture);
  setTextureParameters(renderTarget.textureTarget, *renderTarget.texture());

  if (needsGenerateMipmaps(*renderTarget.texture())) _fn->glGenerateMipmap((GLenum)renderTarget.textureTarget);
}

GLuint Textures::defaultFramebuffer() {
  return _defaultFBO;
}

// Set up GL resources for the render target
void Textures::setupRenderTarget(RenderTargetCube &renderTarget)
{
  auto &textureProperties = _properties.get( renderTarget.texture() );

  renderTarget.onDispose.connect(this, &Textures::onRenderTargetDispose);

  GLuint tex;
  _fn->glGenTextures(1, &tex);
  textureProperties.texture = tex;

  _infoMemory.textures ++;

  // Setup framebuffer
  renderTarget.frameBuffers.resize(6);
  _fn->glGenFramebuffers(6, renderTarget.frameBuffers.data());
  check_glerror(_fn);

  // Setup color buffer
  _state.bindTexture(renderTarget.textureTarget, textureProperties.texture );
  setTextureParameters(renderTarget.textureTarget, *renderTarget.texture());

  for ( unsigned i = 0; i < 6; i ++ ) {
    setupFrameBufferTexture( renderTarget.frameBuffers[ i ], renderTarget, GL_COLOR_ATTACHMENT0, TextureTarget::cubeMapPositiveX + i);
  }

  if ( needsGenerateMipmaps(*renderTarget.texture()) ) _fn->glGenerateMipmap((GLenum)renderTarget.textureTarget);
  _state.bindTexture(renderTarget.textureTarget, 0 );

  // Setup depth and stencil buffers
  if ( renderTarget.depthBuffer() ) {

    setupDepthRenderbuffer( renderTarget );
  }
}

void Textures::updateRenderTargetMipmap(const RenderTarget::Ptr &renderTarget)
{
  if (needsGenerateMipmaps(*renderTarget->texture())) {

    GLuint webglTexture = _properties.get(renderTarget->texture()).texture;

    _state.bindTexture( renderTarget->textureTarget, webglTexture );
    _fn->glGenerateMipmap((GLenum)renderTarget->textureTarget);
    _state.bindTexture( renderTarget->textureTarget, 0 );
  }
}

}
}
