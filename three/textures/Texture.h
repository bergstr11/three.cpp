//
// Created by byter on 06.08.17.
//

#ifndef THREE_QT_TEXTURE_H
#define THREE_QT_TEXTURE_H

#include <QImage>
#include <math/Vector2.h>
#include <vector>
#include <memory>
#include <Constants.h>
#include <helper/simplesignal.h>
#include <helper/sole.h>

namespace three {

struct Mipmap {
  std::vector<unsigned char> data;
  int width, height;
};

class TextureBase
{
public:
  struct Options
  {
    TextureMapping mapping = TextureMapping::UV;

    TextureWrapping wrapS = TextureWrapping::ClampToEdge;
    TextureWrapping wrapT = TextureWrapping::ClampToEdge;

    TextureFilter magFilter = TextureFilter::Linear;
    TextureFilter minFilter = TextureFilter::LinearMipMapLinear;

    float anisotropy = 1;

    TextureFormat format = TextureFormat::RGBA;
    TextureType type = TextureType::UnsignedByte;
    Encoding encoding = Encoding::Linear;

  protected:
    Options() {}
  };
  const sole::uuid uuid;

private:
  bool _needsUpdate;

  std::vector<Mipmap> _mipmaps;

  TextureMapping _mapping;

  TextureWrapping _wrapS;
  TextureWrapping _wrapT;

  TextureFilter _magFilter;
  TextureFilter _minFilter;

  float _anisotropy;

  TextureFormat _format;
  TextureType _type;

  math::Vector2 _offset {0.0f, 0.0f};
  math::Vector2 _repeat {1.0f, 1.0f};

  bool _premultiplyAlpha = false;

  // Values of encoding !== THREE.LinearEncoding only supported on map, envMap and emissiveMap.
  //
  // Also changing the encoding after already used by a Material will not automatically make the Material
  // update.  You need to explicitly call Material.needsUpdate to trigger it to recompile.
  Encoding _encoding = Encoding::Linear;

  unsigned _version = 0;

protected:
  bool _generateMipmaps = true;
  bool _flipY = true;
  unsigned _unpackAlignment = 4;	// valid values: 1, 2, 4, 8 (see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glPixelStorei.xml)

  explicit TextureBase(const Options &options);

public:
  using Ptr = std::shared_ptr<TextureBase>;

  Signal<void(TextureBase *)> onDispose;

  const math::Vector2 &offset() const {return _offset;}
  const math::Vector2 &repeat() const {return _repeat;}

  unsigned version() const {return _version;}

  TextureFormat format() const {return _format;}
  TextureType type() const {return _type;}

  bool flipY() {return _flipY;}

  void dispose() {
    onDispose.emitSignal(this);
  }

  TextureMapping mapping() const {return _mapping;}

  Encoding encoding() const {return _encoding;}

  bool needsUpdate() const {return _needsUpdate;}

  void needsUpdate(bool value) {_needsUpdate = value;}

  bool needsPowerOfTwo()
  {
    return (_wrapS != TextureWrapping ::ClampToEdge || _wrapT != TextureWrapping ::ClampToEdge )
           || (_minFilter != TextureFilter ::Nearest && _minFilter != TextureFilter ::Linear );
  }

  bool needsGenerateMipmaps(bool isPowerOfTwo)
  {
    return _generateMipmaps && isPowerOfTwo && _minFilter != TextureFilter::Nearest
           && _minFilter != TextureFilter ::Linear;
  }
};

class Texture :public TextureBase
{
public:
  struct Options : public TextureBase::Options {};

private:
  QImage _image;

protected:
  Texture(const QImage &image, const Options &options) : TextureBase(options), _image(image) {}
  Texture(const Options &options) : TextureBase(options) {}

public:
  using Ptr = std::shared_ptr<Texture>;
  static Ptr make(const QImage &image, const Options &options) {
    return Ptr(new Texture(image, options));
  }
  static Ptr make(const Options &options) {
    return Ptr(new Texture(options));
  }

  QImage &image() {return _image;}
};

}


#endif //THREE_QT_TEXTURE_H
