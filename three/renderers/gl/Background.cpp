//
// Created by byter on 20.10.17.
//

#include "Background.h"
#include "Renderer_impl.h"

namespace three  {
namespace gl {

void Background::render(RenderList *renderList, const Scene::Ptr scene, const Camera::Ptr camera, bool forceClear)
{
  scene::Functions functions;
  functions._void = [&] () {

    setClear( clearColor, clearAlpha );
    if ( renderer.autoClear || forceClear ) {
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
  };
  functions.color = [this](Color &color) {

    setClear( color, 1 );
    renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
  };
  functions.cubeTexture = [&](CubeTexture::Ptr &tex) {

    if ( renderer.autoClear || forceClear ) {
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
    if (!boxMesh) {

      geometry::BoxBuffer::Ptr box = geometry::BoxBuffer::make(1, 1, 1);
      boxMesh = MeshBase<ShaderMaterial>::make(box,
                                           ShaderMaterial::make(shaderlib::cube, Side::Back, true, false, false));

      box->setNormal(nullptr);
      box->setUV(nullptr);

      boxMesh->onBeforeRender.connect([&] () {

        boxMesh->matrixWorld().setPosition( camera->matrixWorld() );
      });

      geometries.update(box);
    }

    boxMesh->material<0>()->uniforms.set<tCube>(tex);

    renderList->push_back(boxMesh, boxMesh->geometry(), boxMesh->material(), 0, nullptr);
  };
  functions.texture = [&] (Texture::Ptr &tex) {

    if ( renderer.autoClear || forceClear ) {
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
    if (!planeCamera) {

      planeCamera = OrtographicCamera::make( - 1, 1, 1, - 1, 0, 1 );

      MeshBasicMaterial::Ptr mat = MeshBasicMaterial::make();
      mat->depthTest = false;
      mat->depthWrite = false;
      mat->fog = false;

      auto planeGeom = geometry::buffer::Plane::make( 2, 2 );
      planeMesh = MeshBase<MeshBasicMaterial>::make(planeGeom, mat);

      geometries.update(planeGeom);
    }

    planeMesh->material<0>()->map = tex;

    // TODO Push this to renderList
    renderer.renderBufferDirect( planeCamera, nullptr, planeMesh->geometry(), planeMesh->material(), planeMesh, Group());
  };
  scene->resolver->call(functions);
}

}
}

