//
// Created by byter on 12/30/17.
//

#ifndef THREEPP_IMAGECUBETEXTURE_H
#define THREEPP_IMAGECUBETEXTURE_H

#include <QQmlListProperty>

#include "ImageTexture.h"

namespace three {
namespace quick {

class ImageCubeTexture : public Texture
{
Q_OBJECT
  Q_PROPERTY(QQmlListProperty<three::quick::Image> images READ images)

  QList<Image *> _images;

  static void append_object(QQmlListProperty<Image> *list, Image *obj);
  static int count_objects(QQmlListProperty<Image> *);
  static Image *object_at(QQmlListProperty<Image> *, int);
  static void clear_objects(QQmlListProperty<Image> *);

  QQmlListProperty<Image> images();

  three::ImageCubeTexture::Ptr _texture;

protected:
  virtual TextureOptions textureOptions() {
    return three::ImageCubeTexture::options();
  }

public:
  ImageCubeTexture(QObject *parent = nullptr) : Texture(parent)
  {
    TextureOptions options = three::ImageCubeTexture::options();
    _format = (Format)options.format;
    _mapping = (Mapping)options.mapping;
    _minFilter = (Filter)options.minFilter;
    _magFilter = (Filter)options.magFilter;
    _type = (Type)options.type;
    _flipY = options.flipY;
  }

  three::ImageCubeTexture::Ptr getImageCubeTexture();

  three::CubeTexture::Ptr getCubeTexture() override {
    return getImageCubeTexture();
  }

  three::Texture::Ptr getTexture() override {
    return getImageCubeTexture();
  }

  void addTo(ObjectRootContainer *container) override
  {
    container->addTexture(this);
  }

  void setUniform(gl::UniformValues &uniforms, gl::UniformName name) override
  {
    three::CubeTexture::Ptr tex = getImageCubeTexture();
    uniforms.set(name, tex);
  }
};

}
}

#endif //THREEPP_IMAGECUBETEXTURE_H
