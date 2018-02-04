//
// Created by byter on 29.09.17.
//

#ifndef THREE_QT_BACKGROUND_H
#define THREE_QT_BACKGROUND_H

#include <three/core/Color.h>
#include <three/objects/Mesh.h>
#include <three/scene/Scene.h>
#include <three/camera/OrtographicCamera.h>
#include <three/geometry/Box.h>
#include <three/geometry/Plane.h>
#include <three/material/ShaderMaterial.h>
#include "State.h"
#include "Geometries.h"
#include "RenderLists.h"
#include "shader/ShaderLib.h"

namespace three {
namespace gl {

class Renderer_impl;

class Background
{
  Color clearColor;
  float clearAlpha;

  Camera::Ptr planeCamera;
  MeshT<geometry::buffer::Plane, MeshBasicMaterial>::Ptr planeMesh;
  MeshT<geometry::buffer::Box, ShaderMaterial>::Ptr boxMesh;

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
#endif //THREE_QT_BACKGROUND_H
