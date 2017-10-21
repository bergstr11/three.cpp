//
// Created by byter on 21.10.17.
//

#ifndef THREE_QT_FLARERENDERER_H
#define THREE_QT_FLARERENDERER_H

#include <QOpenGLFunctions>
#include "State.h"
#include "Textures.h"
#include "Capabilities.h"

namespace three {
namespace gl {

class FlareRenderer
{
  QOpenGLFunctions * const _fn;
  State &_state;
  Textures &_textures;
  Capabilities &_capabilities;

public:
  FlareRenderer(QOpenGLFunctions * fn, State &state, Textures &textures, Capabilities &capabilities)
  : _fn(fn), _state(state), _textures(textures), _capabilities(capabilities) {}

  void render(std::vector<LensFlare::Ptr> &flares, Scene::Ptr scene, Camera::Ptr camera, const math::Vector4 &viewport);
};

}
}

#endif //THREE_QT_FLARERENDERER_H
