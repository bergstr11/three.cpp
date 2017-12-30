//
// Created by byter on 12/30/17.
//

#ifndef THREEPP_IMAGETEXTURE_H
#define THREEPP_IMAGETEXTURE_H

#include <QUrl>
#include "Texture.h"
#include <textures/ImageTexture.h>
#include <quick/Three.h>

namespace three {
namespace quick {

class Image : public QObject {
Q_OBJECT
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

public:
  QImage getImage()
  {
    QImage image(_url);
    return image.convertToFormat((QImage::Format)_format);
  }
};

class ImageTexture : public Texture
{
Q_OBJECT
  Q_PROPERTY(Image * image READ image WRITE setImage NOTIFY imageChanged)

  Image *_image;

  three::ImageTexture::Ptr _texture;

  three::ImageTexture::Ptr getTexture()
  {
    if(!_texture) {
      TextureOptions options = createTextureOptions();
      _texture = three::ImageTexture::make(options, _image->getImage());
    }
    return _texture;
  }

public:
  Image *image() const {return _image;}
  void setImage(Image *image) {
    if(_image != image) {
      _image = image;
      emit imageChanged();
    }
  }

  void addTo(ObjectRootContainer *container) override
  {
    container->addTexture(getTexture());
  }

  void setUniform(gl::UniformValues &uniforms, gl::UniformName name) override
  {
    three::Texture::Ptr tex = getTexture();
    uniforms.set(name, tex);
  }

signals:
  void imageChanged();
};

}
}

#endif //THREEPP_IMAGETEXTURE_H
