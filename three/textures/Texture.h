//
// Created by byter on 06.08.17.
//

#ifndef THREE_QT_TEXTURE_H
#define THREE_QT_TEXTURE_H

#include <QImage>
#include <math/Vector2.h>
#include <vector>
#include <Constants>

namespace three {

struct Mipmap {
  std::vector<unsigned char> data;
  int width, height;
};

struct TextureData {
  QImage image;

  TextureMapping mapping = TextureMapping::UV;

  TextureWrapping wrapS = TextureWrapping::ClampToEdge;
  TextureWrapping wrapT = TextureWrapping::ClampToEdge;

  TextureFilter magFilter = TextureFilter::Linear;
  TextureFilter minFilter = TextureFilter::LinearMipMapLinear;

  float anisotropy = 1;

  TextureFormat format = TextureFormat::RGBA;
  TextureType type = TextureType::UnsignedByte;
  Encoding encoding = Encoding::Linear;
};

class Texture : private TextureData
{
  QImage image;
  std::vector<Mipmap> mipmaps;

  TextureMapping mapping = TextureMapping::UV;

  TextureWrapping wrapS = TextureWrapping::ClampToEdge;
  TextureWrapping wrapT = TextureWrapping::ClampToEdge;

  TextureFilter magFilter = TextureFilter::Linear;
  TextureFilter minFilter = TextureFilter::LinearMipMapLinear;

  float anisotropy = 1;

  TextureFormat format = TextureFormat::RGBA;
  TextureType type = TextureType::UnsignedByte;

  math::Vector2 offset {0.0f, 0.0f};
  math::Vector2 repeat {1.0f, 1.0f};

  bool generateMipmaps = true;
  bool premultiplyAlpha = false;
  bool flipY = true;
  unsigned unpackAlignment = 4;	// valid values: 1, 2, 4, 8 (see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glPixelStorei.xml)

  // Values of encoding !== THREE.LinearEncoding only supported on map, envMap and emissiveMap.
  //
  // Also changing the encoding after already used by a Material will not automatically make the Material
  // update.  You need to explicitly call Material.needsUpdate to trigger it to recompile.
  Encoding encoding = Encoding::Linear;

  unsigned version = 0;

public:
  Texture(const TextureData &data);
};

}


#endif //THREE_QT_TEXTURE_H
