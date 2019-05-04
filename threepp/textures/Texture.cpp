//
// Created by byter on 06.08.17.
//

#include "DepthTexture.h"

namespace three {

Texture::Texture(const TextureOptions &options, const texture::Typer &typer,
                 bool generateMipMaps, unsigned unpackAlignment)
   : TextureOptions(options),
     typer(typer),
     _generateMipmaps(generateMipMaps),
     _unpackAlignment(unpackAlignment),
     uuid(sole::uuid1())
{
}

DepthTexture::DepthTexture(const TextureOptions &options, size_t width, size_t height)
   : Texture(options, texture::Typer(this), false, false), _width(width), _height(height)
{
  if(options.format != TextureFormat::Depth && options.format != TextureFormat::DepthStencil )
    throw std::invalid_argument("DepthTexture format must be either Depth or DepthStencil");

  if(options.format == TextureFormat::DepthStencil)
    TextureOptions::type = TextureType::UnsignedInt248;
}

void Texture::updateMatrix()
{
  _matrix.setUvTransform( _offset.x(), _offset.y(), _repeat.x(), _repeat.y(), _rotation, _center.x(), _center.y() );
}

void Texture::transformUv(UV &uv)
{
  if (mapping() != TextureMapping::UV) return;

  uv.apply( _matrix );

  if ( uv.x() < 0 || uv.x() > 1 ) {

    switch ( wrapS ) {

      case TextureWrapping::Repeat:
        uv.x() = uv.x() - std::floor( uv.x() );
        break;

      case TextureWrapping::ClampToEdge:
        uv.x() = uv.x() < 0 ? 0 : 1;
        break;

      case TextureWrapping::MirroredRepeat:
        int abs = std::abs((int)std::floor( uv.x()) % 2);
        if ( abs == 1 ) {

          uv.x() = std::ceil( uv.x() ) - uv.x();
        }
        else {
          uv.x() = uv.x() - std::floor( uv.x() );
        }
        break;
    }
  }

  if ( uv.y() < 0 || uv.y() > 1 ) {

    switch ( wrapT ) {

      case TextureWrapping::Repeat:

        uv.y() = uv.y() - std::floor( uv.y() );
        break;

      case TextureWrapping::ClampToEdge:

        uv.y() = uv.y() < 0 ? 0 : 1;
        break;

      case TextureWrapping::MirroredRepeat:
        int abs = std::abs((int)std::floor( uv.y() ) % 2);
        if ( abs == 1 ) {

          uv.y() = std::ceil( uv.y() ) - uv.y();
        }
        else {
          uv.y() = uv.y() - std::floor( uv.y() );
        }
        break;
    }
  }

  if ( flipY ) {
    uv.y() = 1 - uv.y();
  }
}

}