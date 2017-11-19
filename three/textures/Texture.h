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

namespace three {

struct Mipmap {
  std::vector<unsigned char> data;
  int width, height;
};

struct TextureOptions
{
  TextureMapping mapping = TextureMapping::UV;

  TextureWrapping wrapS = TextureWrapping::ClampToEdge;
  TextureWrapping wrapT = TextureWrapping::ClampToEdge;

  TextureFilter magFilter = TextureFilter::Linear;
  TextureFilter minFilter = TextureFilter::LinearMipMapLinear;

  float anisotropy = 1;

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

class Texture : protected TextureOptions
{
  friend class Textures;

protected:
  unsigned _version = 0;
  bool _needsUpdate;

  std::vector<Mipmap> _mipmaps;

  math::Vector2 _offset {0.0f, 0.0f};
  math::Vector2 _repeat {1.0f, 1.0f};
  math::Vector2 _center {0.0f, 0.0f};
  float _rotation = 0;

  bool _matrixAutoUpdate = true;
  math::Matrix3 _matrix;

  bool _premultiplyAlpha = false;

protected:
  bool _generateMipmaps;
  const bool _flipY;
  const unsigned _unpackAlignment = 4;	// valid values: 1, 2, 4, 8 (see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glPixelStorei.xml)

  Texture(texture::Resolver::Ptr resolver,
          const TextureOptions &options,
          bool generateMipMaps=true, bool flipY=true, unsigned unpackAlignment=4);

public:
  static TextureOptions options() {return TextureOptions();}

  using Ptr = std::shared_ptr<Texture>;

  texture::Resolver::Ptr resolver;

  const sole::uuid uuid;
  Signal<void(Texture *)> onDispose;

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

  bool &generateMipmaps() {return _generateMipmaps;}

  bool flipY() {return _flipY;}

  bool premultiplyAlpha() const {return _premultiplyAlpha;}
  unsigned unpackAlignment() const {return _unpackAlignment;}

  void dispose() {
    onDispose.emitSignal(this);
  }

  TextureMapping mapping() const {return TextureOptions::mapping;}

  Encoding encoding() const {return TextureOptions::encoding;}

  bool needsUpdate() const {return _needsUpdate;}

  void needsUpdate(bool value) {_needsUpdate = value;}

  bool needsPowerOfTwo()
  {
    return TextureOptions::wrapS != TextureWrapping ::ClampToEdge || TextureOptions::wrapT != TextureWrapping ::ClampToEdge
           || (TextureOptions::minFilter != TextureFilter ::Nearest && TextureOptions::minFilter != TextureFilter ::Linear );
  }

  bool needsGenerateMipmaps(bool isPowerOfTwo)
  {
    return _generateMipmaps && isPowerOfTwo && TextureOptions::minFilter != TextureFilter::Nearest
           && TextureOptions::minFilter != TextureFilter ::Linear;
  }
};

class DefaultTexture :public Texture
{
private:
  QImage _image;

protected:
  DefaultTexture(const TextureOptions &options, const QImage &image)
     : Texture(texture::ResolverT<DefaultTexture>::make(*this), options), _image(image) {}
  DefaultTexture(const TextureOptions &options)
     : Texture(texture::ResolverT<DefaultTexture>::make(*this), options) {}

public:
  using Ptr = std::shared_ptr<DefaultTexture>;
  static Ptr make(const QImage &image, const TextureOptions &options) {
    return Ptr(new DefaultTexture(options, image));
  }
  static Ptr make(const TextureOptions &options) {
    return Ptr(new DefaultTexture(options));
  }

  QImage &image() {return _image;}
};

}


#endif //THREE_QT_TEXTURE_H
