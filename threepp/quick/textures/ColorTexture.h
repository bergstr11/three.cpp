//
// Created by byter on 3/29/19.
//

#ifndef THREE_PP_COLORTEXTURE_H
#define THREE_PP_COLORTEXTURE_H

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QRect>
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
  Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
  Q_PROPERTY(QRect fillRect READ fillRect WRITE setFillRect NOTIFY fillRectChanged)

  unsigned _width = 1, _height = 1;
  QColor _color {Qt::white}, _fillColor {Qt::black};
  QRect _fillRect {0, 0, -1, -1};

  three::ImageTexture::Ptr _texture;

  three::ImageTexture::Ptr getImageTexture()
  {
    if(!_texture) {
      TextureOptions options = createTextureOptions();
      QImage image(_width, _height, QImage::Format_RGBA8888);

      if(_fillRect.isValid()) {
        image.fill(_color);

        QPainter painter(&image);
        painter.fillRect(_fillRect, _fillColor);
        painter.end();
      }
      else {
        image.fill(_color);
      }

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
  const QColor &fillColor() const {return _fillColor;}
  void setFillColor(const QColor &color) {
    if(_fillColor != color) {
      _fillColor = color;
      emit fillColorChanged();
    }
  }
  const QRect &fillRect() const {return _fillRect;}
  void setFillRect(const QRect &fillRect) {
    if(_fillRect != fillRect) {
      _fillRect = fillRect;
      emit fillRectChanged();
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
  void fillColorChanged();
  void fillRectChanged();
};

}
}

#endif //THREE_PP_COLORTEXTURE_H
