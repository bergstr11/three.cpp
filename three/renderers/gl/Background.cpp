//
// Created by byter on 20.10.17.
//

#include "Background.h"
#include "Renderer_impl.h"

namespace three  {
namespace gl {

void Background::render(RenderList *renderList, const Scene::Ptr scene, const Camera::Ptr camera, bool forceClear)
{
  scene::BackgroundDispatch textures;
  textures.func<nullptr_t>() = [&] (nullptr_t &) {

    setClear( clearColor, clearAlpha );
    if ( renderer.autoClear || forceClear ) {
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
  };
  textures.func<Color>() = [this](Color &color) {

    setClear( color, 1 );
    renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
  };
  textures.func<CubeTexture::Ptr>() = [&](CubeTexture::Ptr &tex) {

    if ( renderer.autoClear || forceClear ) {
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
    if (!boxMesh) {

      geometry::BoxBuffer::Ptr box = geometry::BoxBuffer::make(1, 1, 1);
      ShaderInfo si = shaderlib::get(ShaderID::cube);
      ShaderMaterial::Ptr sm = ShaderMaterial::make(
         si.uniforms, si.vertexShader, si.fragmentShader, Side::Back, true, false, false);

      boxMesh = Mesh_T<geometry::BoxBuffer, ShaderMaterial>::make(box, sm);

      box->setNormal(nullptr);
      box->setUV(nullptr);

      boxMesh->onBeforeRender.connect([&] () {

        boxMesh->matrixWorld().setPosition( camera->matrixWorld() );
      });

      geometries.update(box);
    }

    uniformslib::UniformValue & uv = boxMesh->material<0>()->uniforms[UniformName::cube];// = tex;

    renderList->push_back(boxMesh, boxMesh->geometry_t(), boxMesh->material(), 0, nullptr);
  };
  textures.func<Texture::Ptr>() = [&] (Texture::Ptr &tex) {

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
      planeMesh = Mesh_T<geometry::buffer::Plane, MeshBasicMaterial>::make(planeGeom, mat);

      geometries.update(planeGeom);
    }

    planeMesh->material<0>()->map = tex;

    // TODO Push this to renderList
    renderer.renderBufferDirect( planeCamera, nullptr, planeMesh->geometry_t(), planeMesh->material(), planeMesh, nullptr);
  };
  scene->backgroundResolver->resolver::Resolve<scene::BackgroundDispatch>::getValue(textures);
}

}
}

