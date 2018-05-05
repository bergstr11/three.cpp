//
// Created by byter on 23.09.17.
//

#ifndef THREEPP_RENDERER_H
#define THREEPP_RENDERER_H

#include <threepp/textures/Texture.h>
#include <threepp/scene/Scene.h>
#include <threepp/camera/Camera.h>
#include <threepp/util/sole.h>
#include <threepp/math/Vector4.h>

namespace three {

class Renderer
{
public:
  class Target
  {
  protected:
    Target(GLsizei width, GLsizei height)
       : uuid(sole::uuid0()), _width(width), _height(height), _viewport(0, 0, width, height), _scissor(0, 0, width,  height)
    {}

    GLsizei _width;
    GLsizei _height;

    math::Vector4 _scissor;
    bool _scissorTest = false;

    math::Vector4 _viewport;

  public:
    const sole::uuid uuid;

    using Ptr = std::shared_ptr<Target>;

    const math::Vector4 &scissor() const {return _scissor;}
    bool scissorTest() const {return _scissorTest;}
    const math::Vector4 &viewport() const {return _viewport;}

    GLsizei width() const {return _width;}
    GLsizei height() const {return _height;}

    virtual void init(Renderer *renderer) {}
    virtual Texture::Ptr texture() const = 0;

    Target &setSize(GLsizei width, GLsizei height)
    {
      if ( _width != width || _height != height ) {

        _width = width;
        _height = height;

        //onDispose.emitSignal(this);
      }

      _viewport.set( 0, 0, width, height );
      _scissor.set( 0, 0, width, height );
      return *this;
    }

    Target &setViewport(unsigned x, unsigned y, GLsizei width, GLsizei height)
    {
      _viewport.set( x, y, width, height );
      return *this;
    }
  };

protected:
  virtual ~Renderer() = default;

  virtual void doRender(const Scene::Ptr &scene,
                        const Camera::Ptr &camera,
                        const Target::Ptr &renderTarget,
                        bool forceClear) = 0;

public:
  using Ptr = std::shared_ptr<Renderer>;

  void render(const Scene::Ptr scene,
              const Camera::Ptr camera,
              const Target::Ptr renderTarget=nullptr,
              bool forceClear=false)
  {
    doRender(scene, camera, renderTarget, forceClear);
  }

  virtual Renderer &setClearColor(const Color &color, float alpha) = 0;

  virtual Renderer &setSize(size_t width, size_t height, bool setViewport) = 0;

  virtual Renderer &setViewport(size_t x, size_t y, size_t width, size_t height) = 0;
};

}
#endif //THREEPP_RENDERER_H
