//
// Created by byter on 20.10.17.
//

#include "Background.h"
#include "Renderer_impl.h"
#include <threepp/geometry/Box.h>
#include <threepp/geometry/Plane.h>
#include <threepp/material/ShaderMaterial.h>

namespace three  {
namespace gl {

using namespace std;

struct PlaneMesh : public Mesh
{
  geometry::buffer::Plane::Ptr plane;
  MeshBasicMaterial::Ptr material;

  PlaneMesh(geometry::buffer::Plane::Ptr plane, MeshBasicMaterial::Ptr material)
     : Mesh(plane, {material}), plane(plane), material(material) {}
};

struct BoxMesh : public Mesh
{
  geometry::buffer::Box::Ptr box;
  ShaderMaterial::Ptr material;

  BoxMesh(geometry::buffer::Box::Ptr box, ShaderMaterial::Ptr material)
     : Mesh(box, {material}), box(box), material(material) {}
};

void Background::render(RenderList *renderList, const Scene::Ptr scene, const Camera::Ptr camera, bool forceClear)
{
  if(scene->hasBackground()) {
    if(three::Background<Color> *bg = scene->background()) {

      setClear( bg->data, 1 );
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
    else if(three::Background<Texture::Ptr> *bg = scene->background()) {

      if ( renderer.autoClear || forceClear ) {
        renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
      }

      if(ImageCubeTexture *ict = bg->data->typer) {

        if (!boxMesh) {

          //TODO: find out why we have to pass 100000 rather than 1 as in original code
          geometry::buffer::Box::Ptr box = geometry::buffer::Box::make(100000, 100000, 100000);
          ShaderInfo si = shaderlib::get(ShaderID::cube);
          ShaderMaterial::Ptr sm = ShaderMaterial::make(
             si.uniforms, si.vertexShader, si.fragmentShader, Side::Back, true, false, false);

          boxMesh = make_shared<BoxMesh>(box, sm);

          box->setNormal(nullptr);
          box->setUV(nullptr);

          boxMesh->onBeforeRender.connect([&] (Renderer &renderer, ScenePtr scene, CameraPtr camera,
                                               Object3D &opbject, const Group *group) {
            boxMesh->matrixWorld().setPosition( camera->matrixWorld() );
          });

          geometries.update(box);
        }

        boxMesh->material->uniforms.set(UniformName::tCube, CAST2(bg->data, CubeTexture));

        renderList->push_front(boxMesh, boxMesh->box, boxMesh->material, 0, nullptr);
      }
      else if(ImageTexture *ict = bg->data->typer) {

        if (!planeCamera) {

          planeCamera = OrthographicCamera::make( -1, 1, 1, -1, 0, 1 );

          MeshBasicMaterial::Ptr mat = MeshBasicMaterial::make();
          mat->depthTest = false;
          mat->depthWrite = false;
          mat->fog = false;

          auto planeGeom = geometry::buffer::Plane::make( 2, 2 );
          planeMesh = make_shared<PlaneMesh>(planeGeom, mat);

          geometries.update(planeGeom);
        }

        planeMesh->material->map = bg->data;

        // TODO Push this to renderList
        renderer.renderBufferDirect( planeCamera, nullptr, planeMesh->plane, planeMesh->material, planeMesh, nullptr);
      }
    }
  }
  else {
    setClear( clearColor, clearAlpha );
    if ( renderer.autoClear || forceClear ) {
      renderer.clear( renderer.autoClearColor, renderer.autoClearDepth, renderer.autoClearStencil );
    }
  }
}

}
}

