//
// Created by byter on 29.07.17.
//

#include "OpenGLRenderer_impl.h"

namespace three {

OpenGLRenderer::Ptr OpenGLRenderer::make(QOpenGLContext *context, float width, float height, const OpenGLRendererOptions &options)
{
  return std::shared_ptr<OpenGLRenderer>(new OpenGLRenderer_impl(context, width, height));
}

OpenGLRenderer_impl::OpenGLRenderer_impl(QOpenGLContext *context, unsigned width, unsigned height)
   : OpenGLRenderer(context), state(context) {}

void OpenGLRenderer_impl::render(Scene &scene, Camera::Ptr camera)
{
  if ( _isContextLost ) return;

  // reset caching for this frame

  _currentGeometryProgram = 0;
  _currentMaterialId = -1;
  _currentCamera = nullptr;

  // update scene graph
  if (scene.autoUpdate()) scene.updateMatrixWorld();

  // update camera matrices and frustum

  if(!camera->parent()) camera->updateMatrixWorld();

  /*if ( vr.enabled ) {

    camera = vr.getCamera( camera );

  }*/

  _projScreenMatrix = camera->projectionMatrix() * camera->matrixWorldInverse();
  _frustum.set( _projScreenMatrix );

  _lightsArray.clear();
  _shadowsArray.clear();

  _spritesArray.clear();
  _flaresArray.clear();
#if 0
  _localClippingEnabled = this.localClippingEnabled;
  _clippingEnabled = _clipping.init( this.clippingPlanes, _localClippingEnabled, camera );

  currentRenderList = renderLists.get( scene, camera );
  currentRenderList.init();

  projectObject( scene, camera, _this.sortObjects );

  if ( _this.sortObjects) {

    currentRenderList.sort();

  }

  //

  if ( _clippingEnabled ) _clipping.beginShadows();

  shadowMap.render( shadowsArray, scene, camera );

  lights.setup( lightsArray, shadowsArray, camera );

  if ( _clippingEnabled ) _clipping.endShadows();

  //

  _infoRender.frame ++;
  _infoRender.calls = 0;
  _infoRender.vertices = 0;
  _infoRender.faces = 0;
  _infoRender.points = 0;

  if ( renderTarget === undefined ) {

    renderTarget = null;

  }

  this.setRenderTarget( renderTarget );

  //

  background.render( currentRenderList, scene, camera, forceClear );

  // render scene

  var opaqueObjects = currentRenderList.opaque;
  var transparentObjects = currentRenderList.transparent;

  if ( scene.overrideMaterial ) {

    var overrideMaterial = scene.overrideMaterial;

    if ( opaqueObjects.length ) renderObjects( opaqueObjects, scene, camera, overrideMaterial );
    if ( transparentObjects.length ) renderObjects( transparentObjects, scene, camera, overrideMaterial );

  } else {

    // opaque pass (front-to-back order)

    if ( opaqueObjects.length ) renderObjects( opaqueObjects, scene, camera );

    // transparent pass (back-to-front order)

    if ( transparentObjects.length ) renderObjects( transparentObjects, scene, camera );

  }

  // custom renderers

  spriteRenderer.render( spritesArray, scene, camera );
  flareRenderer.render( flaresArray, scene, camera, _currentViewport );

  // Generate mipmap if we're using any kind of mipmap filtering

  if ( renderTarget ) {

    textures.updateRenderTargetMipmap( renderTarget );

  }

  // Ensure depth buffer writing is enabled so it can be cleared on next render

  state.buffers.depth.setTest( true );
  state.buffers.depth.setMask( true );
  state.buffers.color.setMask( true );

  if ( vr.enabled ) {

    vr.submitFrame();

  }
#endif
  // _gl.finish();
}

/*OpenGLRenderer::OpenGLRenderer(QOpenGLContext *context, OpenGLRendererData data)
   : OpenGLRendererData(data), _context(context)
{
}*/

}