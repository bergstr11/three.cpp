//
// Created by byter on 29.09.17.
//

#ifndef THREEPP_BACKGROUND_H
#define THREEPP_BACKGROUND_H

#include <threepp/core/Color.h>
#include <threepp/objects/Mesh.h>
#include <threepp/scene/Scene.h>
#include <threepp/camera/OrthographicCamera.h>
#include "State.h"
#include "Geometries.h"
#include "RenderLists.h"
#include "shader/ShaderLib.h"

namespace three {
namespace gl {

class Renderer_impl;

class BoxMesh;
class PlaneMesh;

class Background
{
  Color clearColor;
  float clearAlpha;

  Camera::Ptr planeCamera;
  std::shared_ptr<PlaneMesh> planeMesh;
  std::shared_ptr<BoxMesh> boxMesh;

  Renderer_impl &renderer;
  State &state;
  Geometries &geometries;
  bool premultipliedAlpha;

  void setClear(const Color &color, float alpha)
  {
    state.colorBuffer.setClear( color.r, color.g, color.b, alpha, premultipliedAlpha );
  }

public:
  Background(Renderer_impl &renderer, State &state, Geometries &geometries, bool premultipiledAlpha)
     : clearColor(0x000000),
       clearAlpha(0),
       renderer(renderer),
       state(state),
       geometries(geometries),
       premultipliedAlpha(premultipiledAlpha)
  {}

  void render(RenderList *renderList, const Scene::Ptr scene, const Camera::Ptr camera=nullptr, bool forceClear=true);

  const Color &getClearColor()
  {
    return clearColor;
  }

  void setClearColor(const Color &color, float alpha=1)
  {
    clearColor = color;
    clearAlpha = alpha;
    setClear( clearColor, clearAlpha );
  }

  float getClearAlpha()
  {
    return clearAlpha;
  }

  void setClearAlpha(float alpha)
  {
    clearAlpha = alpha;
    setClear( clearColor, clearAlpha );
  }
};

}
}
#endif //THREEPP_BACKGROUND_H
