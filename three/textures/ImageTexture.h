//
// Created by byter on 11/23/17.
//

#ifndef THREE_QT_IMAGETEXTURE_H
#define THREE_QT_IMAGETEXTURE_H

#include "Texture.h"

namespace three {

/**
 * template
 * @tparam imageCount
 */
template <unsigned imageCount>
class ImageTextureT : public Texture
{
protected:
  std::array<QImage, imageCount> _images;

  explicit ImageTextureT(texture::Resolver::Ptr resolver, const TextureOptions &options, const std::array<QImage, imageCount> images)
     : Texture(resolver, options), _images(images) {}

public:
  bool isPowerOfTwo() const override {
    return math::isPowerOfTwo(_images[0].width()) && math::isPowerOfTwo(_images[0].height());
  }
};

/**
 * class
 */
class ImageTexture : public ImageTextureT<1>
{
protected:
  explicit ImageTexture(const TextureOptions &options, QImage image=QImage())
     : ImageTextureT(texture::ResolverT<ImageTexture>::make(*this), options, {image})
  {
  }

public:
  using Ptr = std::shared_ptr<ImageTexture>;
  static Ptr make(const TextureOptions &options, const QImage &image) {
    return Ptr(new ImageTexture(options, image));
  }
  static Ptr make(const TextureOptions &options) {
    return Ptr(new ImageTexture(options));
  }

  const QImage &image() const {return _images[0];}

  void setImage(const QImage &image) {
    _images[0] = image;
  }
};

/**
 * class
 */
class ImageCubeTexture : public ImageTextureT<6>
{
protected:

  ImageCubeTexture(const TextureOptions &options, const std::array<QImage, 6> images)
     : ImageTextureT(texture::ResolverT<ImageCubeTexture>::make(*this), options, images)
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
  static Ptr make(const TextureOptions &options, const std::array<QImage, 6> images) {
    return Ptr(new ImageCubeTexture(options, images));
  }

  const QImage &image(unsigned index)
  {
    return _images[index];
  }

  void setImage(const QImage &image, unsigned index) {
    _images[index] = image;
  }

  bool isPowerOfTwo() const override {
    return math::isPowerOfTwo(_images[0].width()) && math::isPowerOfTwo(_images[0].height());
  }
};

}
#endif //THREE_QT_IMAGETEXTURE_H
