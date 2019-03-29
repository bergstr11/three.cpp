//
// Created by byter on 12/30/17.
//

#ifndef THREEPPQ_IMAGETEXTURE_H
#define THREEPPQ_IMAGETEXTURE_H

#include <QUrl>
#include <QVector2D>
#include <QDebug>
#include "Texture.h"
#include <threepp/textures/ImageTexture.h>
#include <threepp/quick/Three.h>

namespace three {
namespace quick {

class ImageTexture;

class Image : public QObject {
Q_OBJECT

friend ImageTexture;

public:
  enum Format {
    Invalid = QImage::Format_Invalid,
    Mono = QImage::Format_Mono,
    MonoLSB = QImage::Format_MonoLSB,
    Indexed8 = QImage::Format_Indexed8,
    RGB32 = QImage::Format_RGB32,
    ARGB32 = QImage::Format_ARGB32,
    ARGB32_Premultiplied = QImage::Format_ARGB32_Premultiplied,
    RGB16 = QImage::Format_RGB16,
    ARGB8565_Premultiplied = QImage::Format_ARGB8565_Premultiplied,
    RGB666 = QImage::Format_RGB666,
    ARGB6666_Premultiplied = QImage::Format_ARGB6666_Premultiplied,
    RGB555 = QImage::Format_RGB555,
    ARGB8555_Premultiplied = QImage::Format_ARGB8555_Premultiplied,
    RGB888 = QImage::Format_RGB888,
    RGB444 = QImage::Format_RGB444,
    ARGB4444_Premultiplied = QImage::Format_ARGB4444_Premultiplied,
    RGBX8888 = QImage::Format_RGBX8888,
    RGBA8888 = QImage::Format_RGBA8888,
    RGBA8888_Premultiplied = QImage::Format_RGBA8888_Premultiplied,
    BGR30 = QImage::Format_BGR30,
    A2BGR30_Premultiplied = QImage::Format_A2BGR30_Premultiplied,
    RGB30 = QImage::Format_RGB30,
    A2RGB30_Premultiplied = QImage::Format_A2RGB30_Premultiplied,
    Alpha8 = QImage::Format_Alpha8,
    Grayscale8 = QImage::Format_Grayscale8
  };
  Q_ENUM(Format)

private:
  Q_PROPERTY(QString url MEMBER _url)
  Q_PROPERTY(Format format MEMBER _format)

  QString _url;
  Format _format = (Format)QImage::Format_RGBA8888;

  QImage _image;

public:
  QImage getImage()
  {
    if(_image.isNull()) {
      _image = QImage(_url).convertToFormat((QImage::Format)_format);
      if(_image.isNull()) qWarning() << "error loading image" << _url;
    }
    return _image;
  }
};

class ImageTexture : public Texture
{
Q_OBJECT
  Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)
  Q_PROPERTY(three::quick::Image::Format imageFormat READ imageFormat WRITE setImageFormat NOTIFY imageFormatChanged)
  Q_PROPERTY(QVector2D repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)

  Image _image;
  QVector2D _repeat {1, 1};

  three::ImageTexture::Ptr _texture;

protected:
  TextureOptions textureOptions() const override {
    return three::ImageTexture::options();
  }

public:
  ImageTexture(QObject *parent = nullptr) : Texture(parent)
  {
    TextureOptions options = three::ImageTexture::options();
    _format = (Format)options.format;
    _mapping = (Mapping)options.mapping;
    _minFilter = (Three::Filter)options.minFilter;
    _magFilter = (Three::Filter)options.magFilter;
    _type = (Type)options.type;
    _flipY = options.flipY;
  }

  ImageTexture(three::ImageTexture::Ptr texture, QObject *parent = nullptr) : Texture(parent)
  {
    _texture = texture;
    _format = (Format)texture->format();
    _mapping = (Mapping)texture->mapping();
    _minFilter = (Three::Filter)texture->minFilter;
    _magFilter = (Three::Filter)texture->magFilter;
    _type = (Type)texture->type();
    _flipY = texture->flipY;
  }

  three::ImageTexture::Ptr getImageTexture()
  {
    if(!_texture) {
      TextureOptions options = createTextureOptions();
      _texture = three::ImageTexture::make(options, _image.getImage());
      _texture->repeat().set(_repeat.x(), _repeat.y());
    }
    return _texture;
  }

  three::Texture::Ptr getTexture() override
  {
    return getImageTexture();
  }

  const QString &image() const {return _image._url;}
  void setImage(QString image) {
    if(_image._url != image) {
      _image._url = image;
      emit imageChanged();
    }
  }

  const Image::Format &imageFormat() const {return _image._format;}
  void setImageFormat(Image::Format imageFormat) {
    if(_image._format != imageFormat) {
      _image._format = imageFormat;
      emit imageFormatChanged();
    }
  }

  const QVector2D &repeat() const {return _repeat;}
  void setRepeat(const QVector2D &repeat) {
    if(_repeat != repeat) {
      _repeat = repeat;
      emit repeatChanged();
    }
  }

  void setUniform(gl::UniformValues &uniforms, gl::UniformName name) override
  {
    three::Texture::Ptr tex = getTexture();
    uniforms.set(name, tex);
  }

signals:
  void imageChanged();
  void imageFormatChanged();
  void repeatChanged();
};

}
}

#endif //THREEPPQ_IMAGETEXTURE_H
