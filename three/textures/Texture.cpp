//
// Created by byter on 06.08.17.
//

#include "DepthTexture.h"

namespace three {

Texture::Texture(texture::Resolver::Ptr resolver,
                 const TextureOptions &options,
                 bool generateMipMaps, bool flipY, unsigned unpackAlignment)
   : TextureOptions(options),
     resolver(resolver),
     _generateMipmaps(generateMipMaps),
     _flipY(flipY),
     _unpackAlignment(unpackAlignment),
     uuid(sole::uuid0())
{

}

DepthTexture::DepthTexture(const TextureOptions &options, size_t width, size_t height)
   : Texture(texture::ResolverT<DepthTexture>::make(*this), options, false, false), width(width), height(height)
{
  if(options.format != TextureFormat::Depth && options.format != TextureFormat::DepthStencil )
    throw std::invalid_argument("DepthTexture format must be either Depth or DepthStencil");

  if(options.format == TextureFormat::DepthStencil)
    TextureOptions::type = TextureType::UnsignedInt248;
}

#if 0
Texture::transformUv(TextureMapping::UV uv)
{

  if ( this.mapping !== UVMapping ) return;

  uv.multiply( this.repeat );
  uv.add( this.offset );

  if ( uv.x < 0 || uv.x > 1 ) {

    switch ( this.wrapS ) {

      case RepeatWrapping:

        uv.x = uv.x - Math.floor( uv.x );
        break;

      case ClampToEdgeWrapping:

        uv.x = uv.x < 0 ? 0 : 1;
        break;

      case MirroredRepeatWrapping:

        if ( Math.abs( Math.floor( uv.x ) % 2 ) === 1 ) {

          uv.x = Math.ceil( uv.x ) - uv.x;

        } else {

          uv.x = uv.x - Math.floor( uv.x );

        }
        break;

    }

  }

  if ( uv.y < 0 || uv.y > 1 ) {

    switch ( this.wrapT ) {

      case RepeatWrapping:

        uv.y = uv.y - Math.floor( uv.y );
        break;

      case ClampToEdgeWrapping:

        uv.y = uv.y < 0 ? 0 : 1;
        break;

      case MirroredRepeatWrapping:

        if ( Math.abs( Math.floor( uv.y ) % 2 ) === 1 ) {

          uv.y = Math.ceil( uv.y ) - uv.y;

        } else {

          uv.y = uv.y - Math.floor( uv.y );

        }
        break;

    }

  }

  if ( this.flipY ) {

    uv.y = 1 - uv.y;

  }
}
#endif
}