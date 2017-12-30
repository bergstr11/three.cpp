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

  three::ImageCubeTexture::Ptr getTexture()
  {
    if(!_texture) {
      TextureOptions options = createTextureOptions();

      std::array<QImage, 6> images;
      for(int i=0; i<6; i++) {
        images[i] = _images[i]->getImage();
      }
      _texture = three::ImageCubeTexture::make(options, images);
    }
    return _texture;
  }

public:
  void addTo(ObjectRootContainer *container) override
  {
    container->addTexture(getTexture());
  }

  void setUniform(gl::UniformValues &uniforms, gl::UniformName name) override
  {
    three::CubeTexture::Ptr tex = getTexture();
    uniforms.set(name, tex);
  }
};

}
}

#endif //THREEPP_IMAGECUBETEXTURE_H
