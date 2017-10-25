//
// Created by byter on 29.07.17.
//

#include "Renderer_impl.h"
#include <renderers/Resolver.h>
#include <math/Math.h>
#include <textures/DataTexture.h>

namespace three {

OpenGLRenderer::Ptr OpenGLRenderer::make(QOpenGLContext *context, float width, float height, const OpenGLRendererOptions &options)
{
  return std::shared_ptr<OpenGLRenderer>(new gl::Renderer_impl(context, width, height));
}

namespace gl {

Renderer_impl::Renderer_impl(QOpenGLContext *context, unsigned width, unsigned height, bool premultipliedAlpha)
   : OpenGLRenderer(context), _state(this), _attributes(this), _objects(_geometries, _infoRender),
     _geometries(_attributes), _extensions(context), _capabilities(this, _extensions, _parameters ),
     _morphTargets(this), _programs(_extensions, _capabilities),
     _background(*this, _state, _geometries, _premultipliedAlpha),
     _textures(this, _extensions, _state, _properties, _capabilities, _infoMemory),
     _bufferRenderer(this, this, _extensions, _infoRender),
     _indexedRenderer(this, this, _extensions, _infoRender),
     _spriteRenderer(*this, _state, _textures, _capabilities),
     _flareRenderer(this, _state, _textures, _capabilities)
{

}

void Renderer_impl::initContext()
{
  initializeOpenGLFunctions();

  _extensions.get({Extension::ARB_depth_texture, Extension::OES_texture_float,
                   Extension::OES_texture_float_linear, Extension::OES_texture_half_float,
                   Extension::OES_texture_half_float_linear, Extension::OES_standard_derivatives,
                   Extension::ANGLE_instanced_arrays});

  if (_extensions.get(Extension::OES_element_index_uint) ) {

    BufferGeometry::MaxIndex = 4294967296;
  }

  _capabilities.init();

  _state.init();
  _currentScissor = _scissor * _pixelRatio;
  _currentViewport = _viewport * _pixelRatio;
  _state.scissor(_currentScissor);
  _state.viewport(_currentViewport);
}

void Renderer_impl::clear(bool color, bool depth, bool stencil)
{
  unsigned bits = 0;

  if (color) bits |= GL_COLOR_BUFFER_BIT;
  if (depth) bits |= GL_DEPTH_BUFFER_BIT;
  if (stencil) bits |= GL_STENCIL_BUFFER_BIT;

  glClear( bits );
}

void Renderer_impl::doRender(const Scene::Ptr &scene, const Camera::Ptr &camera,
                             const Renderer::Target::Ptr &renderTarget, bool forceClear)
{
  if (_isContextLost) return;

  // reset caching for this frame

  _currentGeometryProgram = 0;
  _currentMaterialId = -1;
  _currentCamera = nullptr;

  // update scene graph
  if (scene->autoUpdate()) scene->updateMatrixWorld(false);

  // update camera matrices and frustum

  if (!camera->parent()) camera->updateMatrixWorld(false);

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

  _shadowMap->render(_shadowsArray, scene, camera);

  _lights.setup(_lightsArray, camera);

  if (_clippingEnabled) _clipping.endShadows();

  //

  _infoRender.frame++;
  _infoRender.calls = 0;
  _infoRender.vertices = 0;
  _infoRender.faces = 0;
  _infoRender.points = 0;

  setRenderTarget(renderTarget);
  //
  _background.render(_currentRenderList, scene, camera, forceClear);

  // render scene
  auto opaqueObjects = _currentRenderList->opaque();
  auto transparentObjects = _currentRenderList->transparent();

  // opaque pass (front-to-back order)
  if (opaqueObjects)
    renderObjects(opaqueObjects, scene, camera, scene->overrideMaterial);

  // transparent pass (back-to-front order)
  if (transparentObjects)
    renderObjects(transparentObjects, scene, camera, scene->overrideMaterial);

  // custom renderers
  _spriteRenderer.render(_spritesArray, scene, camera);
  _flareRenderer.render(_flaresArray, scene, camera, _currentViewport);

  // Generate mipmap if we're using any kind of mipmap filtering

  if (renderTarget) {
    _textures.updateRenderTargetMipmap(renderTarget);
  }

  // Ensure depth buffer writing is enabled so it can be cleared on next render
  _state.depthBuffer.setTest(true);
  _state.depthBuffer.setMask(true);
  _state.colorBuffer.setMask(true);

  /*if (vr.enabled) {
    vr.submitFrame();
  }*/
  // _gl.finish();
}

unsigned Renderer_impl::allocTextureUnit()
{
  unsigned textureUnit = _usedTextureUnits;

  if(textureUnit >= _capabilities.maxTextures ) {
    throw std::logic_error("max texture units exceeded");
  }

  _usedTextureUnits += 1;

  return textureUnit;
}

void setTexture2D(Texture::Ptr texture, unsigned slot )
{
  //_textures.setTexture2D( texture, slot );
}

Renderer_impl& Renderer_impl::setRenderTarget(const Renderer::Target::Ptr renderTarget)
{
  _currentRenderTarget = renderTarget;

  auto renderTargetProperties = _properties.get( renderTarget );

  GLuint *__webglFramebuffer = nullptr;
  if (renderTarget && renderTargetProperties.find(PropertyKey::__webglFramebuffer) != renderTargetProperties.end())
  {
    __webglFramebuffer = renderTargetProperties[PropertyKey::__webglFramebuffer].gluintp_value;
    //textures.setupRenderTarget( renderTarget );
  }

  //var framebuffer = null;
  bool isCube = false;
  GLuint framebuffer = UINT_MAX;

  if (__webglFramebuffer) {

    if (renderTarget->isCube) {

      framebuffer = __webglFramebuffer[ renderTarget->activeCubeFace];
      isCube = true;
    }
    else {
      framebuffer = *__webglFramebuffer;
    }

    _currentViewport = renderTarget->viewport();
    _currentScissor = renderTarget->scissor();
    _currentScissorTest = renderTarget->scissorTest();
  }
  else {

    _currentViewport = _viewport * _pixelRatio;
    _currentScissor = _scissor * _pixelRatio;
    _currentScissorTest = _scissorTest;
  }

  if (_currentFramebuffer != framebuffer ) {

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer );
    _currentFramebuffer = framebuffer;
  }

  _state.viewport( _currentViewport );
  _state.scissor( _currentScissor );
  _state.setScissorTest( _currentScissorTest );

  if ( isCube ) {
    auto textureProperties = _properties.get(renderTarget->texture());
    GLenum textarget = textureProperties[PropertyKey::__webglTexture].gluint_value;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + renderTarget->activeCubeFace,
                           textarget, renderTarget->activeMipMapLevel );
  }
}

void Renderer_impl::renderObjects(RenderList::iterator renderIterator, Scene::Ptr scene, Camera::Ptr camera, Material::Ptr overrideMaterial)
{
  while(renderIterator) {

    const RenderItem &renderItem = *renderIterator;

    //var object = renderItem.object;
    //var geometry = renderItem.geometry;
    Material::Ptr material = overrideMaterial ? renderItem.material : overrideMaterial;
    //var group = renderItem.group;

    camera::Functions funcs;
    funcs.array = [&](ArrayCamera &acamera) {
      _currentArrayCamera = std::dynamic_pointer_cast<ArrayCamera>(camera);

      for ( unsigned j = 0; j < acamera.cameraCount; j ++ ) {

        PerspectiveCamera::Ptr camera2 = acamera[ j ];

        if ( renderItem.object->layers().test( camera2->layers() ) ) {

          /*var bounds = camera2->bounds;

          var x = bounds.x * _width;
          var y = bounds.y * _height;
          var width = bounds.z * _width;
          var height = bounds.w * _height;

          _state.viewport( _currentViewport.set( x, y, width, height ).multiplyScalar( _pixelRatio ) );*/

          renderObject( renderItem.object, scene, camera2, renderItem.geometry, material, renderItem.group );
        }
      }
    };
    funcs.base = [&] (Camera &cam) {
      _currentArrayCamera = nullptr;

      renderObject( renderItem.object, scene, camera, renderItem.geometry, material, renderItem.group );
    };
    camera->resolver->call(funcs);
  }
}

void Renderer_impl::renderObject(Object3D::Ptr object, Scene::Ptr scene, Camera::Ptr camera, Geometry::Ptr geometry,
                  Material::Ptr material, Group *group)
{
  object->onBeforeRender.emitSignal(*this, scene, camera, geometry, material, group);

  object->modelViewMatrix = camera->matrixWorldInverse() * object->matrixWorld();
  object->normalMatrix = object->modelViewMatrix.normalMatrix();

  object::Functions func;
  /*func.immediate = [&] (ImmediateRenderObject &iro) {
    _state.setMaterial( material );

    var program = setProgram( camera, scene->fog(), material, object );

    _currentGeometryProgram = '';

    renderObjectImmediate( object, program, material );
  };*/
  func._void = [&] () {
    renderBufferDirect( camera, scene->fog().get(), geometry, material, object, group );
  };
  object->resolver->call(func);

  object->onAfterRender.emitSignal(*this, scene, camera, geometry, material, group );
}

void Renderer_impl::projectObject(Object3D::Ptr object, Camera::Ptr camera, bool sortObjects )
{
  if (!object->visible()) return;

  bool visible = object->layers().test(camera->layers());
#if 0
  if ( visible ) {

    object::Functions funcs;

    funcs.light = [&] (Light &light) {
      _lightsArray.push( object );

      if ( light.castShadow() ) {
        _shadowsArray.push( object );
      }
    };
    if ( object.isLight ) {
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
#endif
}

void Renderer_impl::renderBufferDirect(Camera::Ptr camera,
                                       const Fog *fog,
                                       Geometry::Ptr geometry,
                                       Material::Ptr material,
                                       Object3D::Ptr object,
                                       const Group *group)
{
#if 0
  state.setMaterial( material );

  var program = setProgram( camera, fog, material, object );
  var geometryProgram = geometry.id + '_' + program.id + '_' + ( material.wireframe === true );

  var updateBuffers = false;

  if ( geometryProgram !== _currentGeometryProgram ) {

    _currentGeometryProgram = geometryProgram;
    updateBuffers = true;

  }

  if ( object.morphTargetInfluences ) {

    morphtargets.update( object, geometry, material, program );

    updateBuffers = true;

  }

  //

  var index = geometry.index;
  var position = geometry.attributes.position;
  var rangeFactor = 1;

  if ( material.wireframe === true ) {

    index = geometries.getWireframeAttribute( geometry );
    rangeFactor = 2;

  }

  var attribute;
  var renderer = bufferRenderer;

  if ( index !== null ) {

    attribute = attributes.get( index );

    renderer = indexedBufferRenderer;
    renderer.setIndex( attribute );

  }

  if ( updateBuffers ) {

    setupVertexAttributes( material, program, geometry );

    if ( index !== null ) {

      _gl.bindBuffer( _gl.ELEMENT_ARRAY_BUFFER, attribute.buffer );

    }

  }

  //

  var dataCount = 0;

  if ( index !== null ) {

    dataCount = index.count;

  } else if ( position !== undefined ) {

    dataCount = position.count;

  }

  var rangeStart = geometry.drawRange.start * rangeFactor;
  var rangeCount = geometry.drawRange.count * rangeFactor;

  var groupStart = group !== null ? group.start * rangeFactor : 0;
  var groupCount = group !== null ? group.count * rangeFactor : Infinity;

  var drawStart = Math.max( rangeStart, groupStart );
  var drawEnd = Math.min( dataCount, rangeStart + rangeCount, groupStart + groupCount ) - 1;

  var drawCount = Math.max( 0, drawEnd - drawStart + 1 );

  if ( drawCount === 0 ) return;

  //

  if ( object.isMesh ) {

    if ( material.wireframe === true ) {

      state.setLineWidth( material.wireframeLinewidth * getTargetPixelRatio() );
      renderer.setMode( _gl.LINES );

    } else {

      switch ( object.drawMode ) {

        case TrianglesDrawMode:
          renderer.setMode( _gl.TRIANGLES );
          break;

        case TriangleStripDrawMode:
          renderer.setMode( _gl.TRIANGLE_STRIP );
          break;

        case TriangleFanDrawMode:
          renderer.setMode( _gl.TRIANGLE_FAN );
          break;

      }

    }


  } else if ( object.isLine ) {

    var lineWidth = material.linewidth;

    if ( lineWidth === undefined ) lineWidth = 1; // Not using Line*Material

    state.setLineWidth( lineWidth * getTargetPixelRatio() );

    if ( object.isLineSegments ) {

      renderer.setMode( _gl.LINES );

    } else if ( object.isLineLoop ) {

      renderer.setMode( _gl.LINE_LOOP );

    } else {

      renderer.setMode( _gl.LINE_STRIP );

    }

  } else if ( object.isPoints ) {

    renderer.setMode( _gl.POINTS );

  }

  if ( geometry && geometry.isInstancedBufferGeometry ) {

    if ( geometry.maxInstancedCount > 0 ) {

      renderer.renderInstances( geometry, drawStart, drawCount );

    }

  } else {

    renderer.render( drawStart, drawCount );

  }
#endif
}

Program::Ptr Renderer_impl::setProgram(Camera::Ptr camera, Fog::Ptr fog, Material::Ptr material, Object3D::Ptr object )
{
#if 0
  _usedTextureUnits = 0;

  auto &materialProperties = _properties.get( material );

  if ( _clippingEnabled ) {

    if ( _localClippingEnabled || camera != _currentCamera ) {

      bool useCache = camera == _currentCamera && material->id() == _currentMaterialId;

      // we might want to call this function with some ClippingGroup
      // object instead of the material, once it becomes feasible
      // (#8465, #8379)
      _clipping.setState(
         material->clippingPlanes, material->clipIntersection, material->clipShadows,
         camera, materialProperties, useCache );
    }
  }

  if (!material->needsUpdate) {

    if (!materialProperties.program) {

      material->needsUpdate = true;

    } else if ( material->fog && materialProperties.fog != fog ) {

      material->needsUpdate = true;

    } else if ( material->lights && materialProperties.lightsHash != _lights.state.hash) {

      material->needsUpdate = true;

    } else if ( materialProperties.numClippingPlanes != nullptr &&
       ( materialProperties.numClippingPlanes != _clipping.numPlanes() ||
          materialProperties.numIntersection != _clipping.numIntersection ) ) {

      material->needsUpdate = true;
    }
  }

  if ( material->needsUpdate ) {

    initMaterial( material, fog, object );
    material->needsUpdate = false;
  }

  bool refreshProgram = false;
  bool refreshMaterial = false;
  bool refreshLights = false;

  Program::Ptr program = materialProperties.program;
  Uniforms::Ptr p_uniforms = program->getUniforms();
  Uniforms::Ptr m_uniforms = materialProperties.shader.uniforms;

  if (_state.useProgram(program->id()) ) {

    refreshProgram = true;
    refreshMaterial = true;
    refreshLights = true;

  }

  if ( material->id() != _currentMaterialId ) {

    _currentMaterialId = material->id();

    refreshMaterial = true;
  }

  if ( refreshProgram || camera != _currentCamera ) {

    p_uniforms->get("projectionMatrix")->setValue(camera->projectionMatrix());

    if (_capabilities.logarithmicDepthBuffer) {

      p_uniforms->get("logDepthBufFC")->setValue(2.0 / ( std::log( camera->far() + 1.0 ) / M_LN2 ) );
    }

    // Avoid unneeded uniform updates per ArrayCamera's sub-camera

    if(_currentArrayCamera && _currentCamera != _currentArrayCamera || _currentCamera != camera) {
      _currentCamera = _currentArrayCamera ? _currentArrayCamera : camera;

      // lighting uniforms depend on the camera so enforce an update
      // now, in case this material supports lights - or later, when
      // the next material that does gets activated:

      refreshMaterial = true;		// set to true on material change
      refreshLights = true;		// remains set until update done
    }

    // load material specific uniforms
    // (shader material also gets them for the sake of genericity)

    resolver::Func<Material> fmat = [&] (Material &mat) {

      Uniform *uCamPos = p_uniforms->get("cameraPosition");

      if(uCamPos) {
        _vector3 = camera->matrixWorld().getPosition();
        uCamPos->setValue(_vector3);
      }
    };
    material::Functions funcs;
    funcs.shader = funcs.meshPhong = funcs.meshStandard = fmat;
    if(!material->resolver->call(funcs) && material->envMap) {
      fmat(*material.get());
    }

    fmat = [&] (Material &mat) {

      p_uniforms->get("viewMatrix")->setValue(camera->matrixWorldInverse() );
    };
    material::Functions funcs2;
    funcs2.meshPhong = funcs2.meshLambert = funcs2.meshBasic = funcs.meshStandard = fmat = funcs2.shader = fmat;
    if(!material->resolver->call(funcs2) && material->skinning) {
      fmat(*material.get());
    }
  }

  // skinning uniforms must be set even if material didn't change
  // auto-setting of texture unit for bone texture must go before other textures
  // not sure why, but otherwise weird things happen

  if ( material->skinning ) {

    object::Functions funcs;
    funcs.skinnedMesh = [&] (SkinnedMesh &m) {
      p_uniforms->get("bindMatrix")->setValue(m.bindMatrix());
      p_uniforms->get("bindMatrixInverse")->setValue(m.bindMatrixInverse());

      if (m.skeleton()) {

        if (_capabilities.floatVertexTextures ) {

          if (m.skeleton()->boneTexture()) {

            // layout (1 matrix = 4 pixels)
            //      RGBA RGBA RGBA RGBA (=> column1, column2, column3, column4)
            //  with  8x8  pixel texture max   16 bones * 4 pixels =  (8 * 8)
            //       16x16 pixel texture max   64 bones * 4 pixels = (16 * 16)
            //       32x32 pixel texture max  256 bones * 4 pixels = (32 * 32)
            //       64x64 pixel texture max 1024 bones * 4 pixels = (64 * 64)


            auto &bones = m.skeleton()->bones();
            float size = std::sqrt( bones.size() * 4 ); // 4 pixels needed for 1 matrix
            size = math::ceilPowerOfTwo( size );
            size = std::max( size, 4 );

            m.skeleton()->boneMatrices().resize(size * size * 4); // 4 floats per RGBA pixel

            DataTexture::Ptr boneTexture = DataTexture::make(m.skeleton()->boneMatrices(),
                                                             size, size, TextureFormat::RGBA, TextureType::Float );

            m.skeleton()->setBoneTexture(boneTexture);
            m.skeleton()->setBoneTextureSize(size);
          }

          p_uniforms->get("boneTexture")->setValue(m.skeleton()->boneTexture() );
          p_uniforms->get("boneTextureSize")->setValue(m.skeleton()->boneTextureSize() );

        } else {
          if(!m.skeleton()->boneMatrices().empty())
            p_uniforms->get("boneMatrices")->setValue(m.skeleton()->boneMatrices().data());
        }
      }
    };
  }

  if ( refreshMaterial ) {

    p_uniforms->get("toneMappingExposure")->setValue(_toneMappingExposure );
    p_uniforms->get("toneMappingWhitePoint")->setValue(_toneMappingWhitePoint );

    if ( material->lights ) {

      // the current material requires lighting info

      // note: all lighting uniforms are always set correctly
      // they simply reference the renderer's state for their
      // values
      //
      // use the current material's .needsUpdate flags to set
      // the GL state when required

      markUniformsLightsNeedsUpdate( m_uniforms, refreshLights );
    }

    // refresh uniforms common to several materials

    if ( fog && material->fog ) {

      refreshUniformsFog( m_uniforms, fog );
    }

    material::Functions mfuncs;
    mfuncs.meshBasic = [&](MeshBasicMaterial &mbm) {
      refreshUniformsCommon( m_uniforms, material );
    };
    mfuncs.meshLambert = [&] (MeshLambertMaterial &mlm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsLambert( m_uniforms, material );
    };
    mfuncs.meshPhong = [&](MeshPhongMaterial &mpm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsPhong( m_uniforms, material );
    };
    mfuncs.meshToon = [&](MeshToonMaterial &mtm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsToon( m_uniforms, material );
    };
    mfuncs.meshStandard = [&] (MeshStandardMaterial &msm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsStandard( m_uniforms, material );
    };
    mfuncs.meshPysical= [&](MeshPhysicalMaterial &mdm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsPhysical( m_uniforms, material );
    };
    mfuncs.meshDepth = [&](MeshDepthMaterial &mdm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsDepth( m_uniforms, material );
    };
    mfuncs.meshDistance = [&](MeshDistanceMaterial &mdm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsDistance( m_uniforms, material );
    };
    mfuncs.meshNormal = [&](MeshNormalMaterial &mnm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsNormal( m_uniforms, material );
    };
    mfuncs.lineBasic = [&](LineBasicMaterial &lbm) {
      refreshUniformsLine( m_uniforms, material );
    };
    mfuncs.lineDashed = [&](LineDashedMaterial &ldm) {
      refreshUniformsLine( m_uniforms, material );
      refreshUniformsDash( m_uniforms, material );
    };
    mfuncs.points = [&] (PointsMaterial &pm) {
      refreshUniformsPoints( m_uniforms, material );
    };
    mfuncs.shadow = [&] (ShadowMaterial &sm) {
      m_uniforms->color.value = material.color;
      m_uniforms->opacity.value = material.opacity;
    };
    material->resolver->call(mfuncs);

    // RectAreaLight Texture
    // TODO (mrdoob): Find a nicer implementation

    if ( m_uniforms.ltcMat !== undefined ) m_uniforms.ltcMat.value = UniformsLib.LTC_MAT_TEXTURE;
    if ( m_uniforms.ltcMag !== undefined ) m_uniforms.ltcMag.value = UniformsLib.LTC_MAG_TEXTURE;

    WebGLUniforms.upload(
       _gl, materialProperties.uniformsList, m_uniforms, _this );
  }

  // common matrices
  p_uniforms->get("modelViewMatrix")->setValue(object->modelViewMatrix );
  p_uniforms->get("normalMatrix")->setValue(object->normalMatrix );
  p_uniforms->get("modelMatrix")->setValue(object->matrixWorld() );

  return program;
#endif
  return nullptr;
}

}
}