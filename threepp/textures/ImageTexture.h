//
// Created by byter on 11/23/17.
//

#ifndef THREEPP_IMAGETEXTURE_H
#define THREEPP_IMAGETEXTURE_H

#include "Texture.h"

namespace three {

/**
 * template
 */
class ImageTexture : public Texture
{
protected:
  QImage _image;

  ImageTexture(const TextureOptions &options, const QImage image)
     : Texture(texture::ResolverT<ImageTexture>::make(*this), options), _image(options.flipY ? image.mirrored() : image) {}

public:
  using Ptr = std::shared_ptr<ImageTexture>;

  static Ptr make(const TextureOptions &options, const QImage &image=QImage()) {
    Ptr p(new ImageTexture(options, image));
    if(!image.isNull()) p->needsUpdate();
    return p;
  }

  const QImage &image() const {return _image;}

  void setImage(const QImage &image) {
    _image = image;
  }

  bool isPowerOfTwo() const override {
    return math::isPowerOfTwo(_image.width()) && math::isPowerOfTwo(_image.height());
  }
};


/**
 * class
 */
class ImageCubeTexture : public CubeTexture
{
  std::array<QImage, CubeTexture::num_faces> _images;

protected:

  ImageCubeTexture(const TextureOptions &options, const std::array<QImage, CubeTexture::num_faces> &images)
     : CubeTexture(texture::ResolverT<ImageCubeTexture>::make(*this), options), _images(images)
  {
  }

public:
  static TextureOptions options()
  {
    TextureOptions options = Texture::options();
    options.mapping = TextureMapping::CubeReflection;
    return options;
  }

  using Ptr = std::shared_ptr<ImageCubeTexture>;

  static Ptr make(const TextureOptions &options, const std::array<QImage, 6> &images) {
    Ptr p(new ImageCubeTexture(options, images));
    p->needsUpdate();
    return p;
  }

  const QImage &image(unsigned index)
  {
    return _images[index];
  }

  void setImage(const QImage &image, unsigned index) {
    _images[index] = image;
  }

  bool dontFlip() const override {return true;}

  bool isPowerOfTwo() const override {
    return math::isPowerOfTwo(_images[0].width()) && math::isPowerOfTwo(_images[0].height());
  }
};

}
#endif //THREEPP_IMAGETEXTURE_H
