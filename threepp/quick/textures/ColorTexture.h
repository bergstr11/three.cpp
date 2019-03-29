//
// Created by byter on 3/29/19.
//

#ifndef THREE_PP_COLORTEXTURE_H
#define THREE_PP_COLORTEXTURE_H

#include <QColor>
#include <QImage>
#include <threepp/textures/ImageTexture.h>

namespace three {
namespace quick {

/**
 * a texture that holds color data
 */
class ColorTexture : public Texture
{
Q_OBJECT
  Q_PROPERTY(unsigned width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(unsigned height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

  unsigned _width = 1, _height = 1;
  QColor _color {Qt::white};

  three::ImageTexture::Ptr _texture;

  three::ImageTexture::Ptr getImageTexture()
  {
    if(!_texture) {
      TextureOptions options = createTextureOptions();
      QImage image(_width, _height, QImage::Format_RGBA8888);
      image.fill(_color);
      _texture = three::ImageTexture::make(options, image);
      _texture->repeat().set(_repeat.x(), _repeat.y());
    }
    return _texture;
  }

public:
  ColorTexture(QObject *parent = nullptr) : Texture(parent) {}

  unsigned width() const {return _width;}
  void setWidth(unsigned width) {
    if(_width != width) {
      _width = width;
      emit widthChanged();
    }
  }
  unsigned height() const {return _height;}
  void setHeight(unsigned height) {
    if(_height != height) {
      _height = height;
      emit heightChanged();
    }
  }
  const QColor &color() const {return _color;}
  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  three::Texture::Ptr getTexture() override
  {
    return getImageTexture();
  }

  void setUniform(gl::UniformValues &uniforms, gl::UniformName name) override
  {
    three::Texture::Ptr tex = getTexture();
    uniforms.set(name, tex);
  }

signals:
  void widthChanged();
  void heightChanged();
  void colorChanged();
};

}
}

#endif //THREE_PP_COLORTEXTURE_H
