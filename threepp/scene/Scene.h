//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_SCENE
#define THREEPP_SCENE

#include <threepp/core/Object3D.h>
#include <threepp/core/Color.h>
#include <threepp/util/Resolver.h>
#include "Fog.h"

namespace three {

/**
 * a scene holds a graph of 3D objects which are rendered together
 */
class Scene : public Object3D
{
  Fog::Ptr _fog;
  bool _autoUpdate;

protected:
  Scene(const Fog::Ptr fog)
     : Object3D(), _fog(fog), _autoUpdate(true) {}

  Scene(const background::Typer &typer)
     : Object3D(), _fog(nullptr), _autoUpdate(true) {}

  Scene(const Scene &scene, const background::Typer &typer)
     : Object3D(scene), _fog(Fog::Ptr(scene._fog->cloned())), _autoUpdate(scene._autoUpdate) {}

public:
  virtual const background::Typer &background() const {
    static const background::Typer bg;
    return bg;
  }

  using Ptr = std::shared_ptr<Scene>;
  static Ptr make() {
    return Ptr(new Scene(background::Typer()));
  }
  
  static Ptr make(std::string name) {
    Ptr p(new Scene(background::Typer()));
    p->_name = name;
    return p;
  }

  Scene *cloned() const override {
    return new Scene(*this);
  }

  virtual bool hasBackground() const {return false;}

  Material::Ptr overrideMaterial;

  const Fog::Ptr fog() const {return _fog;}

  Fog::Ptr &fog() {return _fog;}

  bool autoUpdate() const {return _autoUpdate;}
};

/**
 * background wrapper
 *
 * @tparam _BgData
 */
template <typename _BgData> struct Background
{
  const _BgData data;

  background::Typer typer;

  Background(const _BgData &data)
     : data(data), typer(background::Typer(this)) {}

  Background(const Background &background)
     : data(background.data), typer(background::Typer(this)) {}

  ~Background() {
    qDebug() << "killl";
  }
};

/**
 * scene with background. Currently supported background types are
 * <ul>
 * <li>Color</li>
 * <li>Texture::Ptr</li>
 * </ul>
 * supported Textures are ImageCubetexture and ImageTexture
 *
 * @tparam _Background the background type
 */
template <typename _Background>
class SceneT : public Scene
{
  Background<_Background> _background;

protected:
  SceneT(const _Background &background, const Fog::Ptr &fog)
     : Scene(fog), _background(background) {}
  SceneT(const SceneT &scene)
     : Scene(scene), _background(scene._background) {}

public:
  using Ptr = std::shared_ptr<SceneT<_Background>>;
  static Ptr make(std::string name, const _Background &background, const Fog::Ptr &fog=nullptr) {
    Ptr p(new SceneT(background, fog));
    p->_name = name;
    return p;
  }

  SceneT *cloned() const override {
    return new SceneT(*this);
  }

  bool hasBackground() const override {return true;}

  const background::Typer &background() const override {
    return _background.typer;
  }
};

}

#endif //THREEPP_SCENE
