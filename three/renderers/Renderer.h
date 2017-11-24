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

    virtual Texture::Ptr texture() const = 0;

    Target &setSize(GLsizei width, GLsizei height )
    {
      if ( _width != width || _height != height ) {

        _width = width;
        _height = height;

        //onDispose.emitSignal(this);
      }

      _viewport.set( 0, 0, width, height );
      _scissor.set( 0, 0, width, height );
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
              bool forceClear=true)
  {
    doRender(scene, camera, renderTarget, forceClear);
  }

  virtual Renderer &setClearColor(const Color &color, float alpha) = 0;

  virtual Renderer &setSize(size_t width, size_t height) = 0;
};

}
#endif //THREE_QT_RENDERER_H
