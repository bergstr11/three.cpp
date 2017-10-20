//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_SCENE
#define THREEQT_SCENE

#include <core/Object3D.h>
#include <core/Color.h>
#include <renderers/Resolver.h>
#include "Fog.h"

namespace three {

class Scene : public Object3D
{
  Fog _fog;
  bool _autoUpdate;

protected:
  Scene(scene::ResolverBase::Ptr resolver, const Fog &fog) : _fog(fog) {}
  Scene(scene::ResolverBase::Ptr resolver) {}

public:
  using Ptr = std::shared_ptr<Scene>;
  static Ptr make() {
    return Ptr(new Scene(scene::Resolver<void>::make()));
  }

  scene::ResolverBase::Ptr resolver;

  const Fog &fog() const {return _fog;}

  Fog &fog() {return _fog;}

  bool autoUpdate() const {return _autoUpdate;}
};

template <typename _Background>
class SceneBase : public Scene
{
  _Background _background;

protected:
  SceneBase(typename scene::Resolver<_Background>::Ptr resolver, const _Background &background, const Fog &fog)
     : Scene(resolver, fog), _background(background) {}
  SceneBase(typename scene::Resolver<_Background>::Ptr resolver)
     : Scene(resolver) {}

public:
  using Ptr = std::shared_ptr<SceneBase<_Background>>;
  static Ptr make(const _Background &background, const Fog &fog) {
    return Ptr(new SceneBase(scene::Resolver<_Background>::make(&background), background, fog));
  }

  const _Background &background() const {return _background;}
  _Background &background() {return _background;}
};

}

#endif //THREEQT_SCENE
