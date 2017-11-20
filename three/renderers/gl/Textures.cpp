//
// Created by byter on 02.10.17.
//

#include "Textures.h"
#include <helper/Resolver.h>

namespace three {
namespace gl {

void Textures::onRenderTargetDispose(RenderTarget *renderTarget)
{
  renderTarget->onDispose.disconnectAll();

  deallocateRenderTarget( renderTarget );

  _infoMemory.textures --;
}

void Textures::deallocateTexture(Texture *texture)
{
  if (/*!texture->image().isNull() && */_properties.has(texture, PropertyKey::__image__webglTextureCube)) {

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

  if (_properties.has(renderTarget->texture(), PropertyKey::__webglTexture)) {

    auto textureProperties = _properties.get( renderTarget->texture().get() );
    _fn->glDeleteTextures(1, &textureProperties[PropertyKey::__webglTexture].gluint_value);
  }

  if (renderTarget->depthTexture() ) {

    renderTarget->depthTexture()->dispose();
  }

  int bufcount = renderTarget->bufCount();
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

void Textures::setTexture2D(DefaultTexture::Ptr texture, unsigned slot)
{
  auto &textureProperties = _properties.get(texture->uuid);

  if (texture->version() > 0 && textureProperties[PropertyKey::__version].gluint_value != texture->version() ) {

    if (texture->image().isNull()) {
      throw std::invalid_argument("Texture marked for update but image is undefined");
    }
    else {
      uploadTexture( textureProperties, texture, slot );
      return;
    }
  }
  _state.activeTexture(GL_TEXTURE0 + slot );
  _state.bindTexture(GL_TEXTURE_2D, textureProperties[PropertyKey::__webglTexture].gluint_value);
}

void Textures::setTextureCube(CubeTexture::Ptr texture, unsigned slot)
{
#if 0
  auto textureProperties = _properties.get(texture);

  if ( texture->version() > 0 && textureProperties[PropertyKey::__version].gluint_value != texture->version() ) {

    GLuint webglTextureCube;
    if ( textureProperties.find(PropertyKey::__image__webglTextureCube) == textureProperties.end()) {

      texture->onDispose.connect(onTextureDispose);

      _fn->glGenTextures(1, &webglTextureCube);
      textureProperties[PropertyKey::__image__webglTextureCube] = webglTextureCube;
      _infoMemory.textures ++;
    }
    else
      webglTextureCube = textureProperties[PropertyKey::__image__webglTextureCube].gluint_value;


    _state.activeTexture(GL_TEXTURE0 + slot );
    _state.bindTexture(GL_TEXTURE_CUBE_MAP, webglTextureCube);

    bool isCompressed = texture->isCompressedTexture;
    bool isDataTexture = ( texture.image[ 0 ] && texture.image[ 0 ].isDataTexture );

    QImage cubeImages[6];

    for (unsigned i = 0; i < 6; i ++ ) {
      cubeImages[ i ] = clampToMaxSize( texture->images(i), _capabilities.maxCubemapSize, texture->flipY() );
    }

    //var image = cubeImage[ 0 ],
    bool isPowerOfTwoImage = isPowerOfTwo(cubeImages[0]);
    TextureFormat glFormat = _extensions.extend(texture->format());
    TextureType glType = _extensions.extend(texture->type());

    setTextureParameters(TextureType::CubeMap, texture, isPowerOfTwoImage );

    //texture::Dispatch dispatch;
    //dispatch.func<

    for (unsigned i = 0; i < 6; i ++) {
      if ( ! isCompressed ) {

        if ( isDataTexture ) {

          //TODO: this is the same as the else path..
          _state.texImage2D(TextureTarget::cubeMapPositiveX, i, 0, glFormat,
                            cubeImages[ i ].width(), cubeImages[ i ].height(), glFormat, glType, cubeImages[ i ]);

        } else {

          _state.texImage2D(TextureTarget::cubeMapPositiveX, i, 0, glFormat,
                            cubeImages[ i ].width(), cubeImages[ i ].height(), glFormat, glType, cubeImages[ i ] );
        }
      }
      else {

        var mipmap, mipmaps = cubeImage[ i ].mipmaps;

        for ( var j = 0, jl = mipmaps.length; j < jl; j ++ ) {

          mipmap = mipmaps[ j ];

          if ( texture.format !== RGBAFormat && texture.format !== RGBFormat ) {

            if ( state.getCompressedTextureFormats().indexOf( glFormat ) > - 1 ) {

              state.compressedTexImage2D( _gl.TEXTURE_CUBE_MAP_POSITIVE_X + i, j, glFormat, mipmap.width, mipmap.height, 0, mipmap.data );

            } else {

              console.warn( 'THREE.WebGLRenderer: Attempt to load unsupported compressed texture format in .setTextureCube()' );

            }

          } else {

            state.texImage2D( _gl.TEXTURE_CUBE_MAP_POSITIVE_X + i, j, glFormat, mipmap.width, mipmap.height, 0, glFormat, glType, mipmap.data );

          }

        }

      }

    }

    if ( textureNeedsGenerateMipmaps( texture, isPowerOfTwoImage ) ) {

      _gl.generateMipmap( _gl.TEXTURE_CUBE_MAP );

    }

    textureProperties.__version = texture.version;

    if ( texture.onUpdate ) texture.onUpdate( texture );

  } else {

    state.activeTexture( _gl.TEXTURE0 + slot );
    state.bindTexture( _gl.TEXTURE_CUBE_MAP, textureProperties.__image__webglTextureCube );

  }
#endif
}

void Textures::setTextureCubeDynamic( Texture::Ptr texture, unsigned slot )
{
  _state.activeTexture(GL_TEXTURE0 + slot);
  _state.bindTexture(GL_TEXTURE_CUBE_MAP, (GLint)_properties.get( texture )[PropertyKey::__webglTexture] );
}

void Textures::setTextureParameters(TextureTarget textureTarget, Texture::Ptr texture, bool isPowerOfTwoImage)
{
  //var extension;

  if ( isPowerOfTwoImage ) {

    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_S, (GLint)texture->wrapS);
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_T, (GLint)texture->wrapT);

    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MAG_FILTER, (GLint)texture->magFilter);
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MIN_FILTER, (GLint)texture->minFilter);
  }
  else {
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    if ( texture->wrapS != TextureWrapping::ClampToEdge || texture->wrapT != TextureWrapping::ClampToEdge) {
      //console.warn( 'THREE.WebGLRenderer: Texture is not power of two. Texture.wrapS and Texture.wrapT should be set to THREE.ClampToEdgeWrapping.', texture );
    }

    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MAG_FILTER, filterFallback( texture->magFilter ) );
    _fn->glTexParameteri((GLenum)textureTarget, GL_TEXTURE_MIN_FILTER, filterFallback( texture->minFilter ) );

    if ( texture->minFilter != TextureFilter::Nearest && texture->minFilter != TextureFilter::Linear) {
      //console.warn( 'THREE.WebGLRenderer: Texture is not power of two. Texture.minFilter should be set to THREE.NearestFilter or THREE.LinearFilter.', texture );
    }
  }

  if (_extensions.get(Extension::EXT_texture_filter_anisotropic)) {

    if ( texture->type() == TextureType::Float && !_extensions.get(Extension::OES_texture_float_linear)) return;
    if ( texture->type() == TextureType::HalfFloat && !_extensions.get(Extension::OES_texture_half_float_linear)) return;

    if ( texture->anisotropy > 1 || _properties.has( texture, PropertyKey::__currentAnisotropy)) {

      _fn->glTexParameterf((GLenum)textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                           std::min(texture->anisotropy, (float)_capabilities.getMaxAnisotropy()));
      _properties.get(texture)[PropertyKey::__currentAnisotropy] = texture->anisotropy;
    }
  }
}

void Textures::uploadTexture(Properties::Map textureProperties, DefaultTexture::Ptr texture, unsigned slot )
{

  if ( textureProperties.find(PropertyKey::__webglInit) == textureProperties.end()) {

    textureProperties[PropertyKey::__webglInit] = true;

    texture->onDispose.connect([this](Texture *t) {
      deallocateTexture( t );

      _infoMemory.textures --;
    });

    GLuint tex;
    _fn->glGenTextures(1, &tex);
    textureProperties[PropertyKey::__webglTexture] = tex;

    _infoMemory.textures ++;
  }

  _state.activeTexture( GL_TEXTURE0 + slot );
  _state.bindTexture( GL_TEXTURE_2D, (GLint)textureProperties[PropertyKey::__webglTexture]);

  _fn->glPixelStorei(GL_UNPACK_ALIGNMENT, texture->unpackAlignment() );

  QImage image = clampToMaxSize( texture->image(), _capabilities.maxTextureSize, false);

  if(texture->premultiplyAlpha()) {
    image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    _fn->glBlendEquation(GL_FUNC_ADD); //TODO ?
    _fn->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //TODO ?
  }

  if (!(texture->needsPowerOfTwo() && isPowerOfTwo(image))) {

    image = makePowerOfTwo( image );
  }

  bool isPowerOfTwoImage = isPowerOfTwo( image );

  setTextureParameters(TextureTarget::twoD, texture, isPowerOfTwoImage );

  //var mipmap, mipmaps = texture.mipmaps;

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

    _state.texImage2D(TextureTarget::twoD, 0, internalFormat, image.width(), image.height(), texture.format(), texture.type());
  };
  dispatch.func<DataTexture>() = [&](DataTexture &texture) {
    // use manually created mipmaps if available
    // if there are no manual mipmaps
    // set 0 level mipmap and then use GL to generate other mipmap levels

    if ( texture.mipmaps().size() > 0 && isPowerOfTwoImage ) {

      for ( size_t i = 0, il = texture.mipmaps().size(); i < il; i ++ ) {

        const Mipmap &mipmap = texture.mipmaps()[ i ];
        _state.texImage2D(TextureTarget::twoD, i, texture.format(),
                          mipmap.width, mipmap.height, texture.format(), texture.type(), mipmap.data.data() );
      }

      texture.generateMipmaps() = false;
    }
    else {
      _state.texImage2D(TextureTarget::twoD, 0, texture.format(),
                        image.width(), image.height(), texture.format(), texture.type(), image);
    }
  };
  dispatch.func<CompressedTexture>() = [&](CompressedTexture &texture) {
#if 0
    for ( var i = 0, il = mipmaps.length; i < il; i ++ ) {

      mipmap = mipmaps[ i ];

      if ( texture.format !== RGBAFormat && texture.format !== RGBFormat ) {

        if ( state.getCompressedTextureFormats().indexOf( glFormat ) > - 1 ) {

          state.compressedTexImage2D( _gl.TEXTURE_2D, i, glFormat, mipmap.width, mipmap.height, 0, mipmap.data );

        } else {

          console.warn( 'THREE.WebGLRenderer: Attempt to load unsupported compressed texture format in .uploadTexture()' );

        }

      } else {

        state.texImage2D( _gl.TEXTURE_2D, i, glFormat, mipmap.width, mipmap.height, 0, glFormat, glType, mipmap.data );

      }
    }
#endif
  };
  dispatch.func<Texture>() = [&](Texture &texture) {

    // regular Texture (image, video, canvas)

    // use manually created mipmaps if available
    // if there are no manual mipmaps
    // set 0 level mipmap and then use GL to generate other mipmap levels
#if 0
    if ( mipmaps.length > 0 && isPowerOfTwoImage ) {

      for ( var i = 0, il = mipmaps.length; i < il; i ++ ) {

        mipmap = mipmaps[ i ];
        state.texImage2D( _gl.TEXTURE_2D, i, glFormat, glFormat, glType, mipmap );

      }

      texture.generateMipmaps = false;

    } else {

      state.texImage2D( _gl.TEXTURE_2D, 0, glFormat, glFormat, glType, image );

    }
#endif
  };
  texture->resolver->texture::DispatchResolver::getValue(dispatch);
#if 0
  if ( textureNeedsGenerateMipmaps( texture, isPowerOfTwoImage ) ) _fn->glGenerateMipmap( _gl.TEXTURE_2D );

  textureProperties.__version = texture.version;

  if ( texture.onUpdate ) texture.onUpdate( texture );
#endif
}

#if 0
// Render targets

// Setup storage for target texture and bind it to correct framebuffer
void setupFrameBufferTexture( framebuffer, renderTarget, attachment, textureTarget ) {

  var glFormat = paramThreeToGL( renderTarget.texture.format );
  var glType = paramThreeToGL( renderTarget.texture.type );
  state.texImage2D( textureTarget, 0, glFormat, renderTarget.width, renderTarget.height, 0, glFormat, glType, null );
  _fn->glbindFramebuffer( _gl.FRAMEBUFFER, framebuffer );
  _fn->glframebufferTexture2D( _gl.FRAMEBUFFER, attachment, textureTarget, properties.get( renderTarget.texture ).__webglTexture, 0 );
  _fn->glbindFramebuffer( _gl.FRAMEBUFFER, null );

}

// Setup storage for internal depth/stencil buffers and bind to correct framebuffer
function setupRenderBufferStorage( renderbuffer, renderTarget ) {

  _fn->glbindRenderbuffer( _gl.RENDERBUFFER, renderbuffer );

  if ( renderTarget.depthBuffer && ! renderTarget.stencilBuffer ) {

    _fn->glrenderbufferStorage( _gl.RENDERBUFFER, _gl.DEPTH_COMPONENT16, renderTarget.width, renderTarget.height );
    _gl.framebufferRenderbuffer( _gl.FRAMEBUFFER, _gl.DEPTH_ATTACHMENT, _gl.RENDERBUFFER, renderbuffer );

  } else if ( renderTarget.depthBuffer && renderTarget.stencilBuffer ) {

    _gl.renderbufferStorage( _gl.RENDERBUFFER, _gl.DEPTH_STENCIL, renderTarget.width, renderTarget.height );
    _gl.framebufferRenderbuffer( _gl.FRAMEBUFFER, _gl.DEPTH_STENCIL_ATTACHMENT, _gl.RENDERBUFFER, renderbuffer );

  } else {

    // FIXME: We don't support !depth !stencil
    _gl.renderbufferStorage( _gl.RENDERBUFFER, _gl.RGBA4, renderTarget.width, renderTarget.height );

  }

  _gl.bindRenderbuffer( _gl.RENDERBUFFER, null );

}

// Setup resources for a Depth Texture for a FBO (needs an extension)
function setupDepthTexture( framebuffer, renderTarget ) {

  var isCube = ( renderTarget && renderTarget.isWebGLRenderTargetCube );
  if ( isCube ) throw new Error( 'Depth Texture with cube render targets is not supported' );

  _gl.bindFramebuffer( _gl.FRAMEBUFFER, framebuffer );

  if ( !( renderTarget.depthTexture && renderTarget.depthTexture.isDepthTexture ) ) {

    throw new Error( 'renderTarget.depthTexture must be an instance of THREE.DepthTexture' );

  }

  // upload an empty depth texture with framebuffer size
  if ( !properties.get( renderTarget.depthTexture ).__webglTexture ||
       renderTarget.depthTexture.image.width !== renderTarget.width ||
                                                 renderTarget.depthTexture.image.height !== renderTarget.height ) {
    renderTarget.depthTexture.image.width = renderTarget.width;
    renderTarget.depthTexture.image.height = renderTarget.height;
    renderTarget.depthTexture.needsUpdate = true;
  }

  setTexture2D( renderTarget.depthTexture, 0 );

  var webglDepthTexture = properties.get( renderTarget.depthTexture ).__webglTexture;

  if ( renderTarget.depthTexture.format === DepthFormat ) {

    _gl.framebufferTexture2D( _gl.FRAMEBUFFER, _gl.DEPTH_ATTACHMENT, _gl.TEXTURE_2D, webglDepthTexture, 0 );

  } else if ( renderTarget.depthTexture.format === DepthStencilFormat ) {

    _gl.framebufferTexture2D( _gl.FRAMEBUFFER, _gl.DEPTH_STENCIL_ATTACHMENT, _gl.TEXTURE_2D, webglDepthTexture, 0 );

  } else {

    throw new Error( 'Unknown depthTexture format' );

  }

}

// Setup GL resources for a non-texture depth buffer
function setupDepthRenderbuffer( renderTarget ) {

  var renderTargetProperties = properties.get( renderTarget );

  var isCube = ( renderTarget.isWebGLRenderTargetCube === true );

  if ( renderTarget.depthTexture ) {

    if ( isCube ) throw new Error( 'target.depthTexture not supported in Cube render targets' );

    setupDepthTexture( renderTargetProperties.__webglFramebuffer, renderTarget );

  } else {

    if ( isCube ) {

      renderTargetProperties.__webglDepthbuffer = [];

      for ( var i = 0; i < 6; i ++ ) {

        _gl.bindFramebuffer( _gl.FRAMEBUFFER, renderTargetProperties.__webglFramebuffer[ i ] );
        renderTargetProperties.__webglDepthbuffer[ i ] = _gl.createRenderbuffer();
        setupRenderBufferStorage( renderTargetProperties.__webglDepthbuffer[ i ], renderTarget );

      }

    } else {

      _gl.bindFramebuffer( _gl.FRAMEBUFFER, renderTargetProperties.__webglFramebuffer );
      renderTargetProperties.__webglDepthbuffer = _gl.createRenderbuffer();
      setupRenderBufferStorage( renderTargetProperties.__webglDepthbuffer, renderTarget );

    }

  }

  _gl.bindFramebuffer( _gl.FRAMEBUFFER, null );

}
#endif

// Set up GL resources for the render target
void Textures::setupRenderTarget(RenderTarget &renderTarget)
{
  auto &renderTargetProperties = _properties.get( renderTarget.uuid );
  auto &textureProperties = _properties.get( renderTarget.texture() );
#if 0
  renderTarget.addEventListener( 'dispose', onRenderTargetDispose );

  textureProperties.__webglTexture = _gl.createTexture();

  infoMemory.textures ++;

  var isCube = ( renderTarget.isWebGLRenderTargetCube === true );
  var isTargetPowerOfTwo = isPowerOfTwo( renderTarget );

  // Setup framebuffer

  if ( isCube ) {

    renderTargetProperties.__webglFramebuffer = [];

    for ( var i = 0; i < 6; i ++ ) {

      renderTargetProperties.__webglFramebuffer[ i ] = _gl.createFramebuffer();

    }

  } else {

    renderTargetProperties.__webglFramebuffer = _gl.createFramebuffer();

  }

  // Setup color buffer

  if ( isCube ) {

    state.bindTexture( _gl.TEXTURE_CUBE_MAP, textureProperties.__webglTexture );
    setTextureParameters( _gl.TEXTURE_CUBE_MAP, renderTarget.texture, isTargetPowerOfTwo );

    for ( var i = 0; i < 6; i ++ ) {

      setupFrameBufferTexture( renderTargetProperties.__webglFramebuffer[ i ], renderTarget, _gl.COLOR_ATTACHMENT0, _gl.TEXTURE_CUBE_MAP_POSITIVE_X + i );

    }

    if ( textureNeedsGenerateMipmaps( renderTarget.texture, isTargetPowerOfTwo ) ) _gl.generateMipmap( _gl.TEXTURE_CUBE_MAP );
    state.bindTexture( _gl.TEXTURE_CUBE_MAP, null );

  } else {

    state.bindTexture( _gl.TEXTURE_2D, textureProperties.__webglTexture );
    setTextureParameters( _gl.TEXTURE_2D, renderTarget.texture, isTargetPowerOfTwo );
    setupFrameBufferTexture( renderTargetProperties.__webglFramebuffer, renderTarget, _gl.COLOR_ATTACHMENT0, _gl.TEXTURE_2D );

    if ( textureNeedsGenerateMipmaps( renderTarget.texture, isTargetPowerOfTwo ) ) _gl.generateMipmap( _gl.TEXTURE_2D );
    state.bindTexture( _gl.TEXTURE_2D, null );

  }

  // Setup depth and stencil buffers

  if ( renderTarget.depthBuffer ) {

    setupDepthRenderbuffer( renderTarget );

  }
#endif
}

void Textures::updateRenderTargetMipmap(const Renderer::Target::Ptr &renderTarget)
{
#if 0
  var texture = renderTarget.texture;
  var isTargetPowerOfTwo = isPowerOfTwo( renderTarget );

  if ( textureNeedsGenerateMipmaps( texture, isTargetPowerOfTwo ) ) {

    var target = renderTarget.isWebGLRenderTargetCube ? _gl.TEXTURE_CUBE_MAP : _gl.TEXTURE_2D;
    var webglTexture = properties.get( texture ).__webglTexture;

    state.bindTexture( target, webglTexture );
    _gl.generateMipmap( target );
    state.bindTexture( target, null );

  }
#endif
}

}
}