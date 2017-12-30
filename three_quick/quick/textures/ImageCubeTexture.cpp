//
// Created by byter on 12/30/17.
//

#include "ImageCubeTexture.h"

namespace three {
namespace quick {

void ImageCubeTexture::append_object(QQmlListProperty<Image> *list, Image *obj)
{
  ImageCubeTexture *texture = qobject_cast<ImageCubeTexture *>(list->object);
  if (texture) texture->_images.append(obj);
}

int ImageCubeTexture::count_objects(QQmlListProperty<Image> *list)
{
  ImageCubeTexture *item = qobject_cast<ImageCubeTexture *>(list->object);
  return item ? item->_images.size() : 0;
}

Image *ImageCubeTexture::object_at(QQmlListProperty<Image> *list, int index)
{
  ImageCubeTexture *item = qobject_cast<ImageCubeTexture *>(list->object);
  return item ? item->_images.at(index) : nullptr;
}

void ImageCubeTexture::clear_objects(QQmlListProperty<Image> *list)
{
  ImageCubeTexture *item = qobject_cast<ImageCubeTexture *>(list->object);
  if (item) item->_images.clear();
}

QQmlListProperty<Image> ImageCubeTexture::images()
{
  return QQmlListProperty<Image>(this, nullptr,
                                 &ImageCubeTexture::append_object,
                                 &ImageCubeTexture::count_objects,
                                 &ImageCubeTexture::object_at,
                                 &ImageCubeTexture::clear_objects);
}

}
}