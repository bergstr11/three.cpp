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

void Textures::setTexture2D(Texture::Ptr texture, unsigned slot)
{
  auto textureProperties = _properties.get(texture);

  if (texture->version() > 0 && textureProperties[PropertyKey::__version].gluint_value != texture->version() ) {

    if (texture->image().isNull()) {

      //console.warn( 'THREE.WebGLRenderer: Texture marked for update but image is undefined', texture );

    //} else if (!texture->image().complete()) {

      //console.warn( 'THREE.WebGLRenderer: Texture marked for update but image is incomplete', texture );

    } else {

      uploadTexture( textureProperties, texture, slot );
      return;
    }
  }
  _state.activeTexture(GL_TEXTURE0 + slot );
  _state.bindTexture(GL_TEXTURE_2D, textureProperties[PropertyKey::__webglTexture].gluint_value);
}

void Textures::setTextureCube(Texture::Ptr texture, unsigned slot)
{
  auto textureProperties = _properties.get(texture);

  if ( texture->image().byteCount() == 6) {

    if ( texture->version() > 0 && textureProperties[PropertyKey::__version].gluint_value != texture->version() ) {

      if ( textureProperties.find(PropertyKey::__image__webglTextureCube) == textureProperties.end()) {

        texture->onDispose.connect(onTextureDispose);

        GLuint webglTextureCube;
        _fn->glGenTextures(1, &webglTextureCube);
        textureProperties[PropertyKey::__image__webglTextureCube] = webglTextureCube;
        _infoMemory.textures ++;
      }

      state.activeTexture( _gl.TEXTURE0 + slot );
      state.bindTexture( _gl.TEXTURE_CUBE_MAP, textureProperties.__image__webglTextureCube );

      _gl.pixelStorei( _gl.UNPACK_FLIP_Y_WEBGL, texture.flipY );

      var isCompressed = ( texture && texture.isCompressedTexture );
      var isDataTexture = ( texture.image[ 0 ] && texture.image[ 0 ].isDataTexture );

      var cubeImage = [];

      for ( var i = 0; i < 6; i ++ ) {

        if ( ! isCompressed && ! isDataTexture ) {

          cubeImage[ i ] = clampToMaxSize( texture.image[ i ], capabilities.maxCubemapSize );

        } else {

          cubeImage[ i ] = isDataTexture ? texture.image[ i ].image : texture.image[ i ];

        }

      }

      var image = cubeImage[ 0 ],
         isPowerOfTwoImage = isPowerOfTwo( image ),
         glFormat = paramThreeToGL( texture.format ),
         glType = paramThreeToGL( texture.type );

      setTextureParameters( _gl.TEXTURE_CUBE_MAP, texture, isPowerOfTwoImage );

      for ( var i = 0; i < 6; i ++ ) {

        if ( ! isCompressed ) {

          if ( isDataTexture ) {

            state.texImage2D( _gl.TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glFormat, cubeImage[ i ].width, cubeImage[ i ].height, 0, glFormat, glType, cubeImage[ i ].data );

          } else {

            state.texImage2D( _gl.TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glFormat, glFormat, glType, cubeImage[ i ] );

          }

        } else {

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

  }

}

function setTextureCubeDynamic( texture, slot ) {

  state.activeTexture( _gl.TEXTURE0 + slot );
  state.bindTexture( _gl.TEXTURE_CUBE_MAP, properties.get( texture ).__webglTexture );

}

function setTextureParameters( textureType, texture, isPowerOfTwoImage ) {

  var extension;

  if ( isPowerOfTwoImage ) {

    _gl.texParameteri( textureType, _gl.TEXTURE_WRAP_S, paramThreeToGL( texture.wrapS ) );
    _gl.texParameteri( textureType, _gl.TEXTURE_WRAP_T, paramThreeToGL( texture.wrapT ) );

    _gl.texParameteri( textureType, _gl.TEXTURE_MAG_FILTER, paramThreeToGL( texture.magFilter ) );
    _gl.texParameteri( textureType, _gl.TEXTURE_MIN_FILTER, paramThreeToGL( texture.minFilter ) );

  } else {

    _gl.texParameteri( textureType, _gl.TEXTURE_WRAP_S, _gl.CLAMP_TO_EDGE );
    _gl.texParameteri( textureType, _gl.TEXTURE_WRAP_T, _gl.CLAMP_TO_EDGE );

    if ( texture.wrapS !== ClampToEdgeWrapping || texture.wrapT !== ClampToEdgeWrapping ) {

      console.warn( 'THREE.WebGLRenderer: Texture is not power of two. Texture.wrapS and Texture.wrapT should be set to THREE.ClampToEdgeWrapping.', texture );

    }

    _gl.texParameteri( textureType, _gl.TEXTURE_MAG_FILTER, filterFallback( texture.magFilter ) );
    _gl.texParameteri( textureType, _gl.TEXTURE_MIN_FILTER, filterFallback( texture.minFilter ) );

    if ( texture.minFilter !== NearestFilter && texture.minFilter !== LinearFilter ) {

      console.warn( 'THREE.WebGLRenderer: Texture is not power of two. Texture.minFilter should be set to THREE.NearestFilter or THREE.LinearFilter.', texture );

    }

  }

  extension = extensions.get( 'EXT_texture_filter_anisotropic' );

  if ( extension ) {

    if ( texture.type === FloatType && extensions.get( 'OES_texture_float_linear' ) === null ) return;
    if ( texture.type === HalfFloatType && extensions.get( 'OES_texture_half_float_linear' ) === null ) return;

    if ( texture.anisotropy > 1 || properties.get( texture ).__currentAnisotropy ) {

      _gl.texParameterf( textureType, extension.TEXTURE_MAX_ANISOTROPY_EXT, Math.min( texture.anisotropy, capabilities.getMaxAnisotropy() ) );
      properties.get( texture ).__currentAnisotropy = texture.anisotropy;

    }

  }

}

function uploadTexture( textureProperties, texture, slot ) {

  if ( textureProperties.__webglInit === undefined ) {

    textureProperties.__webglInit = true;

    texture.addEventListener( 'dispose', onTextureDispose );

    textureProperties.__webglTexture = _gl.createTexture();

    infoMemory.textures ++;

  }

  state.activeTexture( _gl.TEXTURE0 + slot );
  state.bindTexture( _gl.TEXTURE_2D, textureProperties.__webglTexture );

  _gl.pixelStorei( _gl.UNPACK_FLIP_Y_WEBGL, texture.flipY );
  _gl.pixelStorei( _gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, texture.premultiplyAlpha );
  _gl.pixelStorei( _gl.UNPACK_ALIGNMENT, texture.unpackAlignment );

  var image = clampToMaxSize( texture.image, capabilities.maxTextureSize );

  if ( textureNeedsPowerOfTwo( texture ) && isPowerOfTwo( image ) === false ) {

    image = makePowerOfTwo( image );

  }

  var isPowerOfTwoImage = isPowerOfTwo( image ),
     glFormat = paramThreeToGL( texture.format ),
     glType = paramThreeToGL( texture.type );

  setTextureParameters( _gl.TEXTURE_2D, texture, isPowerOfTwoImage );

  var mipmap, mipmaps = texture.mipmaps;

  if ( texture.isDepthTexture ) {

    // populate depth texture with dummy data

    var internalFormat = _gl.DEPTH_COMPONENT;

    if ( texture.type === FloatType ) {

      if ( !_isWebGL2 ) throw new Error('Float Depth Texture only supported in WebGL2.0');
      internalFormat = _gl.DEPTH_COMPONENT32F;

    } else if ( _isWebGL2 ) {

      // WebGL 2.0 requires signed internalformat for glTexImage2D
      internalFormat = _gl.DEPTH_COMPONENT16;

    }

    if ( texture.format === DepthFormat && internalFormat === _gl.DEPTH_COMPONENT ) {

      // The error INVALID_OPERATION is generated by texImage2D if format and internalformat are
      // DEPTH_COMPONENT and type is not UNSIGNED_SHORT or UNSIGNED_INT
      // (https://www.khronos.org/registry/webgl/extensions/WEBGL_depth_texture/)
      if ( texture.type !== UnsignedShortType && texture.type !== UnsignedIntType ) {

        console.warn( 'THREE.WebGLRenderer: Use UnsignedShortType or UnsignedIntType for DepthFormat DepthTexture.' );

        texture.type = UnsignedShortType;
        glType = paramThreeToGL( texture.type );

      }

    }

    // Depth stencil textures need the DEPTH_STENCIL internal format
    // (https://www.khronos.org/registry/webgl/extensions/WEBGL_depth_texture/)
    if ( texture.format === DepthStencilFormat ) {

      internalFormat = _gl.DEPTH_STENCIL;

      // The error INVALID_OPERATION is generated by texImage2D if format and internalformat are
      // DEPTH_STENCIL and type is not UNSIGNED_INT_24_8_WEBGL.
      // (https://www.khronos.org/registry/webgl/extensions/WEBGL_depth_texture/)
      if ( texture.type !== UnsignedInt248Type ) {

        console.warn( 'THREE.WebGLRenderer: Use UnsignedInt248Type for DepthStencilFormat DepthTexture.' );

        texture.type = UnsignedInt248Type;
        glType = paramThreeToGL( texture.type );

      }

    }

    state.texImage2D( _gl.TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, glFormat, glType, null );

  } else if ( texture.isDataTexture ) {

    // use manually created mipmaps if available
    // if there are no manual mipmaps
    // set 0 level mipmap and then use GL to generate other mipmap levels

    if ( mipmaps.length > 0 && isPowerOfTwoImage ) {

      for ( var i = 0, il = mipmaps.length; i < il; i ++ ) {

        mipmap = mipmaps[ i ];
        state.texImage2D( _gl.TEXTURE_2D, i, glFormat, mipmap.width, mipmap.height, 0, glFormat, glType, mipmap.data );

      }

      texture.generateMipmaps = false;

    } else {

      state.texImage2D( _gl.TEXTURE_2D, 0, glFormat, image.width, image.height, 0, glFormat, glType, image.data );

    }

  } else if ( texture.isCompressedTexture ) {

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

  } else {

    // regular Texture (image, video, canvas)

    // use manually created mipmaps if available
    // if there are no manual mipmaps
    // set 0 level mipmap and then use GL to generate other mipmap levels

    if ( mipmaps.length > 0 && isPowerOfTwoImage ) {

      for ( var i = 0, il = mipmaps.length; i < il; i ++ ) {

        mipmap = mipmaps[ i ];
        state.texImage2D( _gl.TEXTURE_2D, i, glFormat, glFormat, glType, mipmap );

      }

      texture.generateMipmaps = false;

    } else {

      state.texImage2D( _gl.TEXTURE_2D, 0, glFormat, glFormat, glType, image );

    }

  }

  if ( textureNeedsGenerateMipmaps( texture, isPowerOfTwoImage ) ) _gl.generateMipmap( _gl.TEXTURE_2D );

  textureProperties.__version = texture.version;

  if ( texture.onUpdate ) texture.onUpdate( texture );

}

// Render targets

// Setup storage for target texture and bind it to correct framebuffer
function setupFrameBufferTexture( framebuffer, renderTarget, attachment, textureTarget ) {

  var glFormat = paramThreeToGL( renderTarget.texture.format );
  var glType = paramThreeToGL( renderTarget.texture.type );
  state.texImage2D( textureTarget, 0, glFormat, renderTarget.width, renderTarget.height, 0, glFormat, glType, null );
  _gl.bindFramebuffer( _gl.FRAMEBUFFER, framebuffer );
  _gl.framebufferTexture2D( _gl.FRAMEBUFFER, attachment, textureTarget, properties.get( renderTarget.texture ).__webglTexture, 0 );
  _gl.bindFramebuffer( _gl.FRAMEBUFFER, null );

}

// Setup storage for internal depth/stencil buffers and bind to correct framebuffer
function setupRenderBufferStorage( renderbuffer, renderTarget ) {

  _gl.bindRenderbuffer( _gl.RENDERBUFFER, renderbuffer );

  if ( renderTarget.depthBuffer && ! renderTarget.stencilBuffer ) {

    _gl.renderbufferStorage( _gl.RENDERBUFFER, _gl.DEPTH_COMPONENT16, renderTarget.width, renderTarget.height );
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

// Set up GL resources for the render target
function setupRenderTarget( renderTarget ) {

  var renderTargetProperties = properties.get( renderTarget );
  var textureProperties = properties.get( renderTarget.texture );

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

}

function updateRenderTargetMipmap( renderTarget ) {

  var texture = renderTarget.texture;
  var isTargetPowerOfTwo = isPowerOfTwo( renderTarget );

  if ( textureNeedsGenerateMipmaps( texture, isTargetPowerOfTwo ) ) {

    var target = renderTarget.isWebGLRenderTargetCube ? _gl.TEXTURE_CUBE_MAP : _gl.TEXTURE_2D;
    var webglTexture = properties.get( texture ).__webglTexture;

    state.bindTexture( target, webglTexture );
    _gl.generateMipmap( target );
    state.bindTexture( target, null );

  }
}

}
}