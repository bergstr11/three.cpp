//
// Created by byter on 23.09.17.
//

#ifndef THREE_QT_RENDERER_H
#define THREE_QT_RENDERER_H

#include <textures/Texture.h>
#include <scene/Scene.h>
#include <camera/Camera.h>
#include <helper/sole.h>

namespace three {

class Renderer
{
public:
  class Target
  {
  protected:
    Texture::Ptr _texture;

    Target(Texture::Ptr texture, bool isCube=false) : uuid(sole::uuid0()), _texture(texture), isCube(isCube) {}

  public:
    const sole::uuid uuid;

    const bool isCube;
    unsigned activeCubeFace = 0; // PX 0, NX 1, PY 2, NY 3, PZ 4, NZ 5
    unsigned activeMipMapLevel = 0;

    using Ptr = std::shared_ptr<Target>;

    const Texture::Ptr texture() const {return _texture;}

    virtual const math::Vector4 &scissor() const = 0;
    virtual bool scissorTest() const = 0;
    virtual const math::Vector4 &viewport() const = 0;
  };

protected:
  virtual void doRender(const Scene::Ptr &scene,
                        const Camera::Ptr &camera,
                        const Target::Ptr &renderTarget,
                        bool forceClear) = 0;

public:
  void render(const Scene::Ptr scene,
              const Camera::Ptr camera,
              const Target::Ptr renderTarget=nullptr,
              bool forceClear=true)
  {
    doRender(scene, camera, renderTarget, forceClear);
  }
};

}
#endif //THREE_QT_RENDERER_H
