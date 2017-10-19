//
// Created by byter on 29.09.17.
//

#ifndef THREE_QT_BACKGROUND_H
#define THREE_QT_BACKGROUND_H

#include <core/Color.h>
#include <objects/Mesh.h>
#include <camera/OrtographicCamera.h>
#include <textures/CubeTexture.h>
#include <geometry/Box.h>
#include <geometry/Plane.h>
#include <material/ShaderMaterial.h>
#include "Renderer_impl.h"
#include "shader/ShaderLib.h"

namespace three {
namespace gl {

class Background
{
  Color clearColor;
  float clearAlpha;

  Camera::Ptr planeCamera;
  Mesh::Ptr planeMesh;
  Mesh<ShaderMaterial>::Ptr boxMesh;

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
     : clearColor(0x000000), clearAlpha(0),
       renderer(renderer), state(state), geometries(geometries),
       premultipliedAlpha(premultipiledAlpha)
  {}

  void doRender(RenderList &renderList, const SceneBase::Ptr scene, const Camera::Ptr camera=nullptr, bool forceClear=true)
  {
    scene::Functions functions;
    functions.color = [this](Color *color) {

      setClear( *color, 1 );
      forceClear = true;
    };
    functions.cubeTexture = [this](CubeTexture *tex) {

      if (!boxMesh) {

        // Normalized box
        // 1.1547 = (1,1,1).normalize() * 2.0
        geometry::BoxBuffer::Ptr box = geometry::BoxBuffer::make(1.1547, 1.1547, 1.1547);
        boxMesh = Mesh<ShaderMaterial>::make(box,
           ShaderMaterial::make(ShaderLib::cube, Side::Back, true, false, false));

        box->setNormal(nullptr);
        box->setUV(nullptr);

        boxMesh->onBeforeRender = [&] () {

          float scale = camera->far();

          boxMesh->matrixWorld() = math::Matrix4::scaling( scale, scale, scale );
          boxMesh->matrixWorld().setPosition( camera->matrixWorld() );
        };

        geometries.update(box);
      }

      boxMesh->material<0>()->uniforms.tCube.value = background;

      renderList.push_back(boxMesh, boxMesh->geometry(), boxMesh->material(), 0, -1);
    };
    functions.texture = [this] (Texture *tex) {
      if (!planeCamera) {

        planeCamera = OrtographicCamera::make( - 1, 1, 1, - 1, 0, 1 );

        planeMesh = Mesh::make(
           geometry::buffer::Plane::make( 2, 2 ),
           MeshBasicMaterial::make( { depthTest: false, depthWrite: false, fog: false } )
        );

        geometries.update( planeMesh->geometry() );

      }

      planeMesh.material.map = background;

      // TODO Push this to renderList
      renderer.renderBufferDirect( planeCamera, null, planeMesh->geometry(), planeMesh->material(), planeMesh, null );
    };

    if(!scene->hasBackground()) {
      setClear( clearColor, clearAlpha );
    }
    if ( renderer.autoClear || forceClear ) {
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
    if(scene->hasBackground()) {
      scene->resolver->call(functions);
    }
  }

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
