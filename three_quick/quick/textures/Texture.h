//
// Created by byter on 12/30/17.
//

#ifndef THREEPP_TEXTURE_H
#define THREEPP_TEXTURE_H

#include <quick/ThreeQObjectRoot.h>
#include <renderers/gl/shader/ShaderLib.h>

namespace three {
namespace quick {

class Texture : public ThreeQObjectRoot
{
Q_OBJECT
public:
  enum Format {
    RGB = (unsigned)three::TextureFormat::RGB,
    RGBA = (unsigned)three::TextureFormat::RGBA
  };
  Q_ENUM(Format)

  enum Mapping {
    EquirectangularRefraction = (unsigned)three::TextureMapping::EquirectangularRefraction,
    EquirectangularReflection = (unsigned)three::TextureMapping::EquirectangularReflection,
    SphericalReflection = (unsigned)three::TextureMapping::SphericalReflection,
    CubeReflection = (unsigned)three::TextureMapping::CubeReflection,
    CubeRefraction = (unsigned)three::TextureMapping::CubeRefraction
  };
  Q_ENUM(Mapping)

  enum Filter {
    Linear = (unsigned)three::TextureFilter::Linear,
    LinearMipMapLinear = (unsigned)three::TextureFilter::LinearMipMapLinear
  };
  Q_ENUM(Filter)

  enum Type {
    UnsignedByte = (unsigned)three::TextureType::UnsignedByte,
    UnsignedShort = (unsigned)three::TextureType::UnsignedShort
  };
  Q_ENUM(Type)

private:
  Q_PROPERTY(Format format READ format WRITE setFormat NOTIFY formatChanged)
  Q_PROPERTY(Mapping mapping READ mapping WRITE setMapping NOTIFY mappingChanged)
  Q_PROPERTY(Filter minFilter READ minFilter WRITE setMinFilter NOTIFY minFilterChanged)
  Q_PROPERTY(Filter magFilter READ magFilter WRITE setMagFilter NOTIFY magFilterChanged)
  Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
  Q_PROPERTY(bool flipY READ flipY WRITE setFlipY NOTIFY flipYChanged)

protected:
  Format _format;
  Mapping _mapping;
  Filter _minFilter;
  Filter _magFilter;
  Type _type;
  bool _flipY;

  Texture(QObject *parent = nullptr) : ThreeQObjectRoot(parent) {}

  virtual TextureOptions textureOptions() {
    return three::Texture::options();
  }

public:
  Format format() const {return _format;}
  Mapping mapping() const {return _mapping;}
  Filter minFilter() const {return _minFilter;}
  Filter magFilter() const {return _magFilter;}
  Type type() const {return _type;}
  bool flipY() const {return _flipY;}

  virtual three::Texture::Ptr getTexture() = 0;
  virtual three::CubeTexture::Ptr getCubeTexture() {return nullptr;}

  void setFormat(Format format) {
    if(_format != format) {
      _format = format;
      emit formatChanged();
    }
  }
  void setMapping(Mapping mapping) {
    if(_mapping != mapping) {
      _mapping = mapping;
      three::Texture::Ptr t = getTexture();
      if(t) t->setMapping((three::TextureMapping)mapping);
      emit mappingChanged();
    }
  }
  void setMinFilter(Filter filter) {
    if(_minFilter != filter) {
      _minFilter = filter;
      emit minFilterChanged();
    }
  }
  void setMagFilter(Filter filter) {
    if(_magFilter != filter) {
      _magFilter = filter;
      emit magFilterChanged();
    }
  }
  void setType(Type type) {
    if(_type != type) {
      _type = type;
      emit typeChanged();
    }
  }
  void setFlipY(bool flipY) {
    if(_flipY != flipY) {
      _flipY = flipY;
      emit flipYChanged();
    }
  }

  TextureOptions createTextureOptions()
  {
    TextureOptions options = textureOptions();
    switch(_format) {
      case RGB:
        options.format = TextureFormat::RGB;
        break;
      case RGBA:
        options.format = TextureFormat::RGBA;
        break;
    }
    switch(_mapping) {
      case EquirectangularReflection:
        options.mapping = TextureMapping::EquirectangularReflection;
        break;
      case SphericalReflection:
        options.mapping = TextureMapping::SphericalReflection;
        break;
      case CubeReflection:
        options.mapping = TextureMapping::CubeReflection;
        break;
    }
    switch(_minFilter) {
      case Linear:
        options.minFilter = TextureFilter::Linear;
        break;
      case LinearMipMapLinear:
        options.minFilter = TextureFilter::LinearMipMapLinear;
        break;
    }
    switch(_magFilter) {
      case Linear:
        options.magFilter = TextureFilter::Linear;
        break;
      case LinearMipMapLinear:
        options.magFilter = TextureFilter::LinearMipMapLinear;
        break;
    }
    switch(_type) {
      case UnsignedByte:
        options.type = TextureType::UnsignedByte;
        break;
      case UnsignedShort:
        options.type = TextureType::UnsignedShort;
        break;
    }
    options.flipY = _flipY;

    return options;
  }

  virtual void setUniform(gl::UniformValues &uniforms, gl::UniformName name) = 0;

signals:
  void formatChanged();
  void mappingChanged();
  void minFilterChanged();
  void magFilterChanged();
  void typeChanged();
  void flipYChanged();
};

}
}


#endif //THREEPP_TEXTURE_H
