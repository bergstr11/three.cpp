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

class SceneBase : public Object3D
{
  Fog _fog;
  bool _autoUpdate;

protected:
  SceneBase(scene::ResolverBase::Ptr resolver, const Fog &fog) : _fog(fog) {}
  SceneBase(scene::ResolverBase::Ptr resolver) {}

public:
  using Ptr = std::shared_ptr<SceneBase>;

  scene::ResolverBase::Ptr resolver;

  const Fog &fog() const {return _fog;}

  Fog &fog() {return _fog;}

  bool autoUpdate() const {return _autoUpdate;}
};

template <typename _Background>
class Scene : public SceneBase
{
  _Background _background;

protected:
  Scene(scene::Resolver<_Background>::Ptr resolver, const _Background &background, const Fog &fog)
     : SceneBase(resolver, fog), _background(background) {}
  Scene(scene::Resolver<_Background>::Ptr resolver)
     : SceneBase(resolver) {}

public:
  using Ptr = std::shared_ptr<Scene<_Background>>;
  static Ptr make(const Color &background, const Fog &fog) {
    return Ptr(new Scene(scene::Resolver<_Background>::make(&_background), background, fog));
  }
  static Ptr make() {
    return Ptr(new SceneBase(scene::Resolver<_Background>::make(&_background)));
  }

  const _Background &background() const {return _background;}
  _Background &background() {return _background;}
};

}

#endif //THREEQT_SCENE
