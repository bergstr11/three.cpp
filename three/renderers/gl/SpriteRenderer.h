//
// Created by byter on 21.10.17.
//

#ifndef THREE_QT_SPRITERENDERER_H
#define THREE_QT_SPRITERENDERER_H

#include <QOpenGLFunctions>
#include "State.h"
#include "Textures.h"
#include "Capabilities.h"

namespace three {
namespace gl {

class SpriteRenderer
{
  QOpenGLFunctions * const _fn;
  State &_state;
  Textures &_textures;
  Capabilities &_capabilities;

public:
  SpriteRenderer(QOpenGLFunctions * fn, State &state, Textures &textures, Capabilities &capabilities)
     : _fn(fn), _state(state), _textures(textures), _capabilities(capabilities) {}

  void render(const std::vector<Sprite::Ptr> &sprites, Scene::Ptr scene, Camera::Ptr camera);
};

}
}

#endif //THREE_QT_SPRITERENDERER_H
