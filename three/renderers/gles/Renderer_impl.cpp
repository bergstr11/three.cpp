//
// Created by byter on 29.07.17.
//

#include "Renderer_impl.h"

namespace three {

OpenGLRenderer::Ptr OpenGLRenderer::make(QOpenGLContext *context, float width, float height, const OpenGLRendererOptions &options)
{
  return std::shared_ptr<OpenGLRenderer>(new gl::Renderer_impl(context, width, height));
}

namespace gl {

Renderer_impl::Renderer_impl(QOpenGLContext *context, unsigned width, unsigned height)
   : OpenGLRenderer(context), state(context)
{}

void Renderer_impl::render(const Scene::Ptr scene, const Camera::Ptr camera)
{
  if (_isContextLost) return;

  // reset caching for this frame

  _currentGeometryProgram = 0;
  _currentMaterialId = -1;
  _currentCamera = nullptr;

  // update scene graph
  if (scene->autoUpdate()) scene->updateMatrixWorld();

  // update camera matrices and frustum

  if (!camera->parent()) camera->updateMatrixWorld();

  /*if ( vr.enabled ) {

    camera = vr.getCamera( camera );

  }*/

  _projScreenMatrix = camera->projectionMatrix() * camera->matrixWorldInverse();
  _frustum.set(_projScreenMatrix);

  _lightsArray.clear();
  _shadowsArray.clear();

  _spritesArray.clear();
  _flaresArray.clear();

  _clippingEnabled = _clipping.init(_clippingPlanes, _localClippingEnabled, camera);

  _currentRenderList = _renderLists.get(scene, camera);
  _currentRenderList->init();

  projectObject(scene, camera, _sortObjects);

  if (_sortObjects) {
    _currentRenderList->sort();
  }

  //
  if (_clippingEnabled) _clipping.beginShadows();

  _shadowMap.render(shadowsArray, scene, camera);

  lights.setup(lightsArray, shadowsArray, camera);

  if (_clippingEnabled) _clipping.endShadows();

  //

  _infoRender.frame++;
  _infoRender.calls = 0;
  _infoRender.vertices = 0;
  _infoRender.faces = 0;
  _infoRender.points = 0;

  if (renderTarget == = undefined) {

    renderTarget = null;

  }

  this.setRenderTarget(renderTarget);

  //

  background.render(currentRenderList, scene, camera, forceClear);

  // render scene

  var opaqueObjects = currentRenderList.opaque;
  var transparentObjects = currentRenderList.transparent;

  if (scene.overrideMaterial) {

    var overrideMaterial = scene.overrideMaterial;

    if (opaqueObjects.length) renderObjects(opaqueObjects, scene, camera, overrideMaterial);
    if (transparentObjects.length) renderObjects(transparentObjects, scene, camera, overrideMaterial);

  }
  else {

    // opaque pass (front-to-back order)

    if (opaqueObjects.length) renderObjects(opaqueObjects, scene, camera);

    // transparent pass (back-to-front order)

    if (transparentObjects.length) renderObjects(transparentObjects, scene, camera);

  }

  // custom renderers

  spriteRenderer.render(spritesArray, scene, camera);
  flareRenderer.render(flaresArray, scene, camera, _currentViewport);

  // Generate mipmap if we're using any kind of mipmap filtering

  if (renderTarget) {

    textures.updateRenderTargetMipmap(renderTarget);

  }

  // Ensure depth buffer writing is enabled so it can be cleared on next render

  state.buffers.depth.setTest(true);
  state.buffers.depth.setMask(true);
  state.buffers.color.setMask(true);

  if (vr.enabled) {

    vr.submitFrame();

  }

  // _gl.finish();
}

void Renderer_impl::projectObject(Object3D::Ptr object, Camera::Ptr camera, bool sortObjects )
{
  if (!object->visible()) return;

  bool visible = object->layers().test(camera->layers());

  if ( visible ) {

    if ( object.isLight ) {

      lightsArray.push( object );

      if ( object.castShadow ) {

        shadowsArray.push( object );

      }

    } else if ( object.isSprite ) {

      if ( ! object.frustumCulled || _frustum.intersectsSprite( object ) ) {

        spritesArray.push( object );

      }

    } else if ( object.isLensFlare ) {

      flaresArray.push( object );

    } else if ( object.isImmediateRenderObject ) {

      if ( sortObjects ) {

        _vector3.setFromMatrixPosition( object.matrixWorld )
           .applyMatrix4( _projScreenMatrix );

      }

      currentRenderList.push( object, null, object.material, _vector3.z, null );

    } else if ( object.isMesh || object.isLine || object.isPoints ) {

      if ( object.isSkinnedMesh ) {

        object.skeleton.update();

      }

      if ( ! object.frustumCulled || _frustum.intersectsObject( object ) ) {

        if ( sortObjects ) {

          _vector3.setFromMatrixPosition( object.matrixWorld )
             .applyMatrix4( _projScreenMatrix );

        }

        var geometry = objects.update( object );
        var material = object.material;

        if ( Array.isArray( material ) ) {

          var groups = geometry.groups;

          for ( var i = 0, l = groups.length; i < l; i ++ ) {

            var group = groups[ i ];
            var groupMaterial = material[ group.materialIndex ];

            if ( groupMaterial && groupMaterial.visible ) {

              currentRenderList.push( object, geometry, groupMaterial, _vector3.z, group );

            }

          }

        } else if ( material.visible ) {

          currentRenderList.push( object, geometry, material, _vector3.z, null );

        }

      }

    }

  }

  var children = object.children;

  for ( var i = 0, l = children.length; i < l; i ++ ) {

    projectObject( children[ i ], camera, sortObjects );

  }
}

}
}