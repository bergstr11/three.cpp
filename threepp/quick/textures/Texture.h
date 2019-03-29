//
// Created by byter on 12/30/17.
//

#ifndef THREEPPQ_TEXTURE_H
#define THREEPPQ_TEXTURE_H

#include <QVector2D>
#include <threepp/quick/Three.h>
#include <threepp/quick/ThreeQObjectRoot.h>
#include <threepp/renderers/gl/shader/ShaderLib.h>

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
    UV = (unsigned)three::TextureMapping::UV,
    EquirectangularRefraction = (unsigned)three::TextureMapping::EquirectangularRefraction,
    EquirectangularReflection = (unsigned)three::TextureMapping::EquirectangularReflection,
    SphericalReflection = (unsigned)three::TextureMapping::SphericalReflection,
    CubeReflection = (unsigned)three::TextureMapping::CubeReflection,
    CubeRefraction = (unsigned)three::TextureMapping::CubeRefraction
  };
  Q_ENUM(Mapping)

  enum Type {
    UnsignedByte = (unsigned)three::TextureType::UnsignedByte,
    UnsignedShort = (unsigned)three::TextureType::UnsignedShort
  };
  Q_ENUM(Type)

private:
  Q_PROPERTY(Format format READ format WRITE setFormat NOTIFY formatChanged)
  Q_PROPERTY(Mapping mapping READ mapping WRITE setMapping NOTIFY mappingChanged)
  Q_PROPERTY(three::quick::Three::Filter minFilter READ minFilter WRITE setMinFilter NOTIFY minFilterChanged)
  Q_PROPERTY(three::quick::Three::Filter magFilter READ magFilter WRITE setMagFilter NOTIFY magFilterChanged)
  Q_PROPERTY(three::quick::Three::Wrapping wrapS READ wrapS WRITE setWrapS NOTIFY wrapSChanged)
  Q_PROPERTY(three::quick::Three::Wrapping wrapT READ wrapT WRITE setWrapT NOTIFY wrapTChanged)
  Q_PROPERTY(QVector2D repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
  Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
  Q_PROPERTY(float anisotropy READ anisotropy WRITE setAnisotropy NOTIFY anisotropyChanged)
  Q_PROPERTY(bool flipY READ flipY WRITE setFlipY NOTIFY flipYChanged)

protected:
  Format _format = RGBA;
  Mapping _mapping = UV;
  Three::Filter _minFilter = Three::LinearMipMapLinear;
  Three::Filter _magFilter = Three::LinearFilter;
  Three::Wrapping _wrapS = Three::ClampToEdge, _wrapT = Three::ClampToEdge;
  QVector2D _repeat {1.0f, 1.0f};
  float _anisotropy = 1;
  Type _type = UnsignedByte;
  bool _flipY = true;

  Texture(QObject *parent = nullptr) : ThreeQObjectRoot(parent) {}

  virtual TextureOptions textureOptions() const {
    return three::Texture::options();
  }

public:

  virtual three::Texture::Ptr getTexture() = 0;
  virtual three::CubeTexture::Ptr getCubeTexture() {return nullptr;}

  Format format() const {return _format;}
  void setFormat(Format format) {
    if(_format != format) {
      _format = format;
      emit formatChanged();
    }
  }
  Mapping mapping() const {return _mapping;}
  void setMapping(Mapping mapping) {
    if(_mapping != mapping) {
      _mapping = mapping;
      three::Texture::Ptr t = getTexture();
      if(t) t->setMapping((three::TextureMapping)mapping);
      emit mappingChanged();
    }
  }
  Three::Filter minFilter() const {return _minFilter;}
  void setMinFilter(Three::Filter filter) {
    if(_minFilter != filter) {
      _minFilter = filter;
      emit minFilterChanged();
    }
  }
  Three::Filter magFilter() const {return _magFilter;}
  void setMagFilter(Three::Filter filter) {
    if(_magFilter != filter) {
      _magFilter = filter;
      emit magFilterChanged();
    }
  }
  Type type() const {return _type;}
  void setType(Type type) {
    if(_type != type) {
      _type = type;
      emit typeChanged();
    }
  }
  bool flipY() const {return _flipY;}
  void setFlipY(bool flipY) {
    if(_flipY != flipY) {
      _flipY = flipY;
      emit flipYChanged();
    }
  }
  Three::Wrapping wrapS() const {return _wrapS;}
  void setWrapS(Three::Wrapping wrap) {
    if(_wrapS != wrap) {
      _wrapS = wrap;
      emit wrapSChanged();
    }
  }
  Three::Wrapping wrapT() const {return _wrapT;}
  void setWrapT(Three::Wrapping wrap) {
    if(_wrapT != wrap) {
      _wrapT = wrap;
      emit wrapTChanged();
    }
  }
  float anisotropy() const {return _anisotropy;}
  void setAnisotropy(float anisotropy) {
    if(_anisotropy != anisotropy) {
      _anisotropy = anisotropy;
      emit anisotropyChanged();
    }
  }
  const QVector2D & repeat() const {return _repeat;}
  void setRepeat(const QVector2D &repeat) {
    if(_repeat != repeat) {
      _repeat = repeat;
      emit repeatChanged();
    }
  }

  TextureOptions createTextureOptions() const
  {
    TextureOptions options = textureOptions();
    options.format = (TextureFormat)_format;
    options.mapping = (TextureMapping)_mapping;
    options.minFilter = (TextureFilter)_minFilter;
    options.magFilter = (TextureFilter)_magFilter;
    options.type = (TextureType)_type;
    options.wrapS = (TextureWrapping)_wrapS;
    options.wrapT = (TextureWrapping)_wrapT;
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
  void wrapSChanged();
  void wrapTChanged();
  void anisotropyChanged();
  void repeatChanged();
};

}
}


#endif //THREEPPQ_TEXTURE_H
