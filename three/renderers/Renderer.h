//
// Created by byter on 23.09.17.
//

#ifndef THREE_QT_RENDERER_H
#define THREE_QT_RENDERER_H

#include <textures/Texture.h>
#include <scene/Scene.h>
#include <camera/Camera.h>

namespace three {

class Renderer
{
public:
  class Target
  {
  protected:
    Texture::Ptr _texture;

    Target(Texture::Ptr texture) : _texture(texture) {}

  public:
    using Ptr = std::shared_ptr<Target>;
  };

  virtual void render(const Scene::Ptr scene, const Camera::Ptr camera) = 0;
};

}
#endif //THREE_QT_RENDERER_H
