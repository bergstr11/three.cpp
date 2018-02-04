//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_SCENE
#define THREEQT_SCENE

#include <three/core/Object3D.h>
#include <three/core/Color.h>
#include <three/helper/Resolver.h>
#include "Fog.h"

namespace three {

class Scene : public Object3D
{
  Fog::Ptr _fog;
  bool _autoUpdate;

protected:
  Scene(const Fog::Ptr fog, scene::Resolver::Ptr resolver=scene::Resolver::makeNull())
     : Object3D(nullptr), _fog(fog), _autoUpdate(true), backgroundResolver(resolver) {}
  Scene(scene::Resolver::Ptr resolver=scene::Resolver::makeNull())
     : Object3D(nullptr), _fog(nullptr), _autoUpdate(true), backgroundResolver(resolver) {}

public:
  using Ptr = std::shared_ptr<Scene>;
  static Ptr make() {
    return Ptr(new Scene(scene::Resolver::makeNull()));
  }
  
  static Ptr make(std::string name) {
    Ptr p(new Scene(scene::Resolver::makeNull()));
    p->_name = name;
    return p;
  }

  virtual bool hasBackground() const {return false;}

  scene::Resolver::Ptr backgroundResolver;

  Material::Ptr overrideMaterial;

  const Fog::Ptr fog() const {return _fog;}

  Fog::Ptr &fog() {return _fog;}

  bool autoUpdate() const {return _autoUpdate;}
};

template <typename _Background>
class SceneT : public Scene
{
  _Background _background;

protected:
  SceneT(const _Background &background, const Fog::Ptr &fog)
     : Scene(fog, scene::ResolverT<_Background>::make(_background)), _background(background) {}
  SceneT()
     : Scene(scene::ResolverT<_Background>::make(_background)) {}

public:
  using Ptr = std::shared_ptr<SceneT<_Background>>;
  static Ptr make(std::string name, const _Background &background, const Fog::Ptr &fog=nullptr) {
    Ptr p(new SceneT(background, fog));
    p->_name = name;
    return p;
  }

  bool hasBackground() const override {return true;}

  const _Background &background() const {return _background;}
  _Background &background() {return _background;}
};

}

#endif //THREEQT_SCENE
