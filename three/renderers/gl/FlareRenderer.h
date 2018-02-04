//
// Created by byter on 21.10.17.
//

#ifndef THREE_QT_FLARERENDERER_H
#define THREE_QT_FLARERENDERER_H

#include <QOpenGLFunctions>
#include <three/objects/LensFlare.h>
#include "State.h"
#include "Textures.h"
#include "Capabilities.h"
#include "Program.h"

namespace three {
namespace gl {

class FlareRenderer
{
  class Data;
  Data *_data = nullptr;

  QOpenGLFunctions * const _fn;
  State &_state;
  Textures &_textures;
  Capabilities &_capabilities;
  GLuint _program;
  bool _linked = false;

  GLuint _vertexBuffer;
  GLuint _elementBuffer;
  GLuint _tempTexture;
  GLuint _occlusionTexture;

  void init();

public:
  FlareRenderer(QOpenGLFunctions * fn, State &state, Textures &textures, Capabilities &capabilities)
  : _fn(fn), _state(state), _textures(textures), _capabilities(capabilities) {}

  ~FlareRenderer();

  void render(std::vector<LensFlare::Ptr> &flares, Scene::Ptr scene, Camera::Ptr camera, const math::Vector4 &viewport);
};

}
}

#endif //THREE_QT_FLARERENDERER_H
