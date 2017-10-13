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

    Texture::Ptr texture() {return _texture;}

  };

protected:
  virtual void doRender(const SceneBase::Ptr &scene,
                        const Camera::Ptr &camera,
                        const Target::Ptr &renderTarget,
                        bool forceClear) = 0;

public:
  void render(const SceneBase::Ptr scene,
              const Camera::Ptr camera,
              const Target::Ptr renderTarget=nullptr,
              bool forceClear=true)
  {
    doRender(scene, camera, renderTarget, forceClear);
  }
};

}
#endif //THREE_QT_RENDERER_H
