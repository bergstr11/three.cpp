//
// Created by byter on 06.08.17.
//

#ifndef THREE_QT_TEXTURE_H
#define THREE_QT_TEXTURE_H

#include <QImage>
#include <math/Vector2.h>
#include <math/Matrix3.h>
#include <vector>
#include <memory>
#include <Constants.h>
#include <helper/simplesignal.h>
#include <helper/sole.h>
#include <helper/Resolver.h>
#include <helper/Types.h>

namespace three {

struct TextureOptions
{
  TextureMapping mapping = TextureMapping::UV;

  TextureWrapping wrapS = TextureWrapping::ClampToEdge;
  TextureWrapping wrapT = TextureWrapping::ClampToEdge;

  TextureFilter magFilter = TextureFilter::Linear;
  TextureFilter minFilter = TextureFilter::LinearMipMapLinear;

  float anisotropy = 1;

  // no flipping for cube textures
  // (also flipping doesn't work for compressed textures )
  bool flipY = true;

  TextureFormat format = TextureFormat::RGBA;
  TextureType type = TextureType::UnsignedByte;

  // Values of encoding !== THREE.LinearEncoding only supported on map, envMap and emissiveMap.
  //
  // Also changing the encoding after already used by a Material will not automatically make the Material
  // update.  You need to explicitly call Material.needsUpdate to trigger it to recompile.
  Encoding encoding = Encoding::Linear;

protected:
  friend class Texture;
  TextureOptions() {}
};

class Texture : public TextureOptions
{
  friend class Textures;

protected:
  unsigned _version = 0;

  std::vector<Mipmap> _mipmaps;

  math::Vector2 _offset {0.0f, 0.0f};
  math::Vector2 _repeat {1.0f, 1.0f};
  math::Vector2 _center {0.0f, 0.0f};
  float _rotation = 0;

  bool _matrixAutoUpdate = true;
  math::Matrix3 _matrix;

  bool _premultiplyAlpha = false;

protected:
  // can't generate mipmaps for compressed textures
  // mips must be embedded in DDS files
  bool _generateMipmaps;

  // valid values: 1, 2, 4, 8 (see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glPixelStorei.xml)
  const unsigned _unpackAlignment = 4;

  Texture(texture::Resolver::Ptr resolver,
          const TextureOptions &options,
          bool generateMipMaps=true, unsigned unpackAlignment=4);

public:
  static TextureOptions options() {return TextureOptions();}

  using Ptr = std::shared_ptr<Texture>;

  texture::Resolver::Ptr resolver;

  const sole::uuid uuid;
  Signal<void(Texture &)> onDispose;
  Signal<void(Texture &)> onUpdate;

  const math::Vector2 &offset() const {return _offset;}
  const math::Vector2 &repeat() const {return _repeat;}
  const math::Vector2 &center() const {return _center;}
  float rotation() const {return _rotation;}

  bool matrixAutoUpdate() const {return _matrixAutoUpdate;}

  const math::Matrix3 &matrix() const {return _matrix;}
  math::Matrix3 &matrix() {return _matrix;}

  unsigned version() const {return _version;}

  TextureFormat format() const {return TextureOptions::format;}
  TextureType type() const {return TextureOptions::type;}

  TextureType &type() {return TextureOptions::type;}

  const std::vector<Mipmap> &mipmaps() const {return _mipmaps;}

  bool generateMipmaps() const {return _generateMipmaps;}

  void setGenerateMipmaps(bool generate) {_generateMipmaps = generate;}

  bool premultiplyAlpha() const {return _premultiplyAlpha;}
  unsigned unpackAlignment() const {return _unpackAlignment;}

  void dispose() {
    onDispose.emitSignal(*this);
  }

  TextureMapping mapping() const {return TextureOptions::mapping;}

  Encoding encoding() const {return TextureOptions::encoding;}

  void transformUv(UV &uv);

  void needsUpdate(bool value) {_version ++;}

  virtual bool isPowerOfTwo() const = 0;

  virtual bool dontFlip() const {return false;}

  bool needsPowerOfTwo()
  {
    return TextureOptions::wrapS != TextureWrapping ::ClampToEdge || TextureOptions::wrapT != TextureWrapping ::ClampToEdge
           || (TextureOptions::minFilter != TextureFilter ::Nearest && TextureOptions::minFilter != TextureFilter ::Linear );
  }
};

class CubeTexture : public Texture
{
protected:
  CubeTexture(texture::Resolver::Ptr resolver, const TextureOptions &options, bool generateMipMaps=true,
              unsigned unpackAlignment=4)
     : Texture(resolver, options, generateMipMaps, unpackAlignment) {}

public:
  static constexpr unsigned num_faces = 6;

  using Ptr = std::shared_ptr<CubeTexture>;
};

}


#endif //THREE_QT_TEXTURE_H
