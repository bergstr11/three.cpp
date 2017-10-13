//
// Created by byter on 06.08.17.
//

#include "Texture.h"

namespace three {

TextureBase::TextureBase(const Options &options)
   : _mapping(options.mapping),
     _wrapS(options.wrapS),
     _wrapT(options.wrapT),
     _magFilter(options.magFilter),
     _minFilter(options.minFilter),
     _anisotropy(options.anisotropy),
     _format(options.format),
     _type(options.type),
     _encoding(options.encoding),
     uuid(sole::uuid0())
{

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