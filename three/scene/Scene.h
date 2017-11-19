//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_SCENE
#define THREEQT_SCENE

#include <core/Object3D.h>
#include <core/Color.h>
#include <helper/Resolver.h>
#include "Fog.h"

namespace three {

class Scene : public Object3D
{
  Fog::Ptr _fog;
  bool _autoUpdate;

protected:
  Scene(const Fog::Ptr fog, scene::Resolver::Ptr resolver=scene::NullResolver::make()) : _fog(fog), backgroundResolver(resolver) {}
  Scene(scene::Resolver::Ptr resolver=scene::NullResolver::make()) : backgroundResolver(resolver) {}

public:
  using Ptr = std::shared_ptr<Scene>;
  static Ptr make() {
    return Ptr(new Scene(scene::NullResolver::make()));
  }

  scene::Resolver::Ptr backgroundResolver;

  Material::Ptr overrideMaterial;

  const Fog::Ptr fog() const {return _fog;}

  Fog::Ptr fog() {return _fog;}

  bool autoUpdate() const {return _autoUpdate;}
};

template <typename _Background>
class SceneBase : public Scene
{
  _Background _background;

protected:
  SceneBase(const _Background &background, const Fog::Ptr &fog)
     : Scene(fog, scene::ResolverT<_Background>::make(*this)), _background(background) {}
  SceneBase()
     : Scene(scene::ResolverT<_Background>::make(*this)) {}

public:
  using Ptr = std::shared_ptr<SceneBase<_Background>>;
  static Ptr make(const _Background &background, const Fog::Ptr &fog) {
    return Ptr(new SceneBase(background, fog));
  }

  const _Background &background() const {return _background;}
  _Background &background() {return _background;}
};

}

#endif //THREEQT_SCENE
