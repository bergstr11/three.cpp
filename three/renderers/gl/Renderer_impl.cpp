//
// Created by byter on 29.07.17.
//

#include "Renderer_impl.h"
#include <renderers/Resolver.h>
#include <math/Math.h>
#include <textures/DataTexture.h>
#include <sstream>
#include <objects/Line.h>

namespace three {

using namespace std;

OpenGLRenderer::Ptr OpenGLRenderer::make(QOpenGLContext *context, float width, float height, const OpenGLRendererOptions &options)
{
  return shared_ptr<OpenGLRenderer>(new gl::Renderer_impl(context, width, height));
}

namespace gl {

Renderer_impl::Renderer_impl(QOpenGLContext *context, unsigned width, unsigned height, bool premultipliedAlpha)
   : OpenGLRenderer(context), _state(this), _attributes(this), _objects(_geometries, _infoRender),
     _geometries(_attributes), _extensions(context), _capabilities(this, _extensions, _parameters ),
     _morphTargets(this), _programs(_extensions, _capabilities),
     _background(*this, _state, _geometries, _premultipliedAlpha),
     _textures(this, _extensions, _state, _properties, _capabilities, _infoMemory),
     _bufferRenderer(this, this, _extensions, _infoRender),
     _indexedBufferRenderer(this, this, _extensions, _infoRender),
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

  _currentGeometryProgram.clear();
  _currentMaterialId = -1;
  _currentCamera = nullptr;

  // update scene graph
  if (scene->autoUpdate()) scene->updateMatrixWorld(false);

  // update camera matrices and frustum

  if (!camera->parent()) camera->updateMatrixWorld(false);

  //TODO VR
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

  //TODO VR
  /*if (vr.enabled) {
    vr.submitFrame();
  }*/
  // _gl.finish();
}

unsigned Renderer_impl::allocTextureUnit()
{
  unsigned textureUnit = _usedTextureUnits;

  if(textureUnit >= _capabilities.maxTextures ) {
    throw logic_error("max texture units exceeded");
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
      _currentArrayCamera = dynamic_pointer_cast<ArrayCamera>(camera);

      for ( unsigned j = 0; j < acamera.cameraCount; j ++ ) {

        PerspectiveCamera::Ptr camera2 = acamera[ j ];

        if ( renderItem.object->layers().test( camera2->layers() ) ) {

          //TODO VR
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
  func.immediate = [&] (ImmediateRenderObject &iro) {
    _state.setMaterial( material );

    Program::Ptr program = setProgram( camera, scene->fog(), material, object );

    _currentGeometryProgram.clear();

    renderObjectImmediate( iro, program, material );
  };
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
  if ( visible ) {

    object::Functions funcs;

    funcs.light = [&] (Light &light) {
      Light::Ptr lp = dynamic_pointer_cast<Light>(object);
      _lightsArray.push_back(lp);

      if ( light.castShadow() ) {
        _shadowsArray.push_back( lp );
      }
    };
    funcs.sprite = [&](Sprite &sprite) {
      Sprite::Ptr sp = dynamic_pointer_cast<Sprite>(object);
      if ( ! sprite.frustumCulled || _frustum.intersectsSprite(sp) ) {
        _spritesArray.push_back( sp );
      }
    };
    funcs.lensFlare = [&](LensFlare &flare) {
      LensFlare::Ptr fp = dynamic_pointer_cast<LensFlare>(object);
      _flaresArray.push_back( fp );
    };
    funcs.immediate = [&](ImmediateRenderObject &iro) {
      if ( sortObjects ) {

        _vector3 = object->matrixWorld().getPosition().apply( _projScreenMatrix );
      }
      _currentRenderList->push_back(object, nullptr, object->material(), _vector3.z(), nullptr );
    };
    funcs.mesh = [&](Mesh &mesh) {

      if ( ! object->frustumCulled || _frustum.intersectsObject( object ) ) {

        if ( sortObjects ) {
          _vector3 = object->matrixWorld().getPosition().apply( _projScreenMatrix );
        }

        Geometry::Ptr geometry = _objects.update( object );

        if ( object->materialCount() > 1) {

          const vector<Group> &groups = geometry->groups();

          for (const Group &group : groups) {

            Material::Ptr groupMaterial = object->material(group.materialIndex);

            if ( groupMaterial && groupMaterial->visible ) {

              _currentRenderList->push_back( object, geometry, groupMaterial, _vector3.z(), &group );
            }
          }
        } else {
          Material::Ptr material = object->material();
          if ( material->visible )
            _currentRenderList->push_back( object, geometry, material, _vector3.z(), nullptr);
        }
      }
    };
    funcs.skinnedMesh = [&](SkinnedMesh &sk) {
      sk.skeleton()->update();
      funcs.mesh(sk);
    };
    funcs.line = funcs.mesh;
    funcs.points = funcs.mesh;

    object->resolver->call(funcs);
  }

  for (Object3D::Ptr child : object->children()) {

    projectObject( child, camera, sortObjects );
  }
}

void Renderer_impl::renderBufferDirect(Camera::Ptr camera,
                                       Fog::Ptr fog,
                                       BufferGeometry::Ptr geometry,
                                       Material::Ptr material,
                                       Object3D::Ptr object,
                                       const Group *group)
{
  _state.setMaterial( material );

  Program::Ptr program = setProgram( camera, fog, material, object );
  
  stringstream ss;
  ss << geometry->id << '_' << program->id() << '_' << material->wireframe;
  string geometryProgram = ss.str();

  bool updateBuffers = false;

  if (geometryProgram != _currentGeometryProgram ) {

    _currentGeometryProgram = geometryProgram;
    updateBuffers = true;
  }

  Mesh::Ptr mesh = dynamic_pointer_cast<Mesh>(object);
  if ( mesh && mesh->morphTargetInfluences ) {

    _morphTargets.update( mesh, geometry, material, program );

    updateBuffers = true;
  }

  BufferAttributeBase<uint32_t>::Ptr index;
  unsigned rangeFactor = 1;
  BufferRenderer *renderer;

  if ( material->wireframe ) {
    index = _geometries.getWireframeAttribute( geometry );
    rangeFactor = 2;
  }
  else {
    index = geometry->index();
  }

  if ( updateBuffers )
    setupVertexAttributes( material, program, geometry );

  if (geometry->index()) {

    const Buffer &attribute = _attributes.get( *geometry->index() );

    if ( updateBuffers )
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attribute.buf);

    _indexedBufferRenderer.setIndex( attribute );
    renderer = &_indexedBufferRenderer;
  }
  else {
    renderer = &_bufferRenderer;
  }

  //
  size_t dataCount = 0;

  if (index) {

    dataCount = index->count();
  }
  else if (geometry->position()) {

    dataCount = geometry->position()->count();
  }

  auto rangeStart = geometry->drawRange().offset * rangeFactor;
  auto rangeCount = geometry->drawRange().count * rangeFactor;

  size_t groupStart = group ? group->start * rangeFactor : 0;
  size_t groupCount = group ? group->count * rangeFactor : numeric_limits<size_t>::infinity();

  size_t drawStart = std::max( rangeStart, groupStart );
  size_t drawEnd = std::min( dataCount, rangeStart + rangeCount, groupStart + groupCount ) - 1;

  size_t drawCount = std::max( 0, drawEnd - drawStart + 1 );

  if ( drawCount == 0 ) return;

  //
  object::Functions funcs;
  funcs.mesh = [&] (Mesh &mesh) {
    if ( material->wireframe ) {

      _state.setLineWidth( material->wireframeLineWidth * getTargetPixelRatio() );
      renderer->setMode(DrawMode::Lines);

    } else {

      renderer->setMode(mesh.drawMode());
    }
  };
  funcs.line = [&] (Line &line) {
    //var lineWidth = material.linewidth;
    //if ( lineWidth === undefined ) lineWidth = 1; // Not using Line*Material

    _state.setLineWidth( line.material<0>()->linewidth * getTargetPixelRatio() );

    renderer->setMode(DrawMode::LineStrip);

    //TODO implement classes
    /*if ( line.isLineSegments ) {

      renderer.setMode( _gl.LINES );

    } else if ( object.isLineLoop ) {

      renderer.setMode( _gl.LINE_LOOP );

    }*/
  };
  funcs.points = [&](Points &points) {

    renderer->setMode(DrawMode::Points);
  };

  InstancedBufferGeometry::Ptr ibg = dynamic_pointer_cast<InstancedBufferGeometry>(geometry);
  if (ibg) {
    if ( ibg->maxInstancedCount() > 0 ) {
      renderer->renderInstances( geometry, drawStart, drawCount );
    }
  } else {
    renderer->render( drawStart, drawCount );
  }
}

void Renderer_impl::initMaterial(Material::Ptr material, Fog::Ptr fog, Object3D::Ptr object)
{
  auto &materialProperties = _properties.get( material );

  var parameters = _programs.getParameters(
     material, lights.state, _shadowsArray, fog, _clipping.numPlanes, _clipping.numIntersection, object );

  var code = programCache.getProgramCode( material, parameters );

  var program = materialProperties.program;
  var programChange = true;

  if ( program === undefined ) {

    // new material
    material.addEventListener( 'dispose', onMaterialDispose );

  } else if ( program.code !== code ) {

    // changed glsl or parameters
    releaseMaterialProgramReference( material );

  } else if ( parameters.shaderID !== undefined ) {

    // same glsl and uniform list
    return;

  } else {

    // only rebuild uniform list
    programChange = false;

  }

  if ( programChange ) {

    if ( parameters.shaderID ) {

      var shader = ShaderLib[ parameters.shaderID ];

      materialProperties.shader = {
         name: material.type,
         uniforms: UniformsUtils.clone( shader.uniforms ),
         vertexShader: shader.vertexShader,
         fragmentShader: shader.fragmentShader
      };

    } else {

      materialProperties.shader = {
         name: material.type,
         uniforms: material.uniforms,
         vertexShader: material.vertexShader,
         fragmentShader: material.fragmentShader
      };

    }

    material.onBeforeCompile( materialProperties.shader );

    program = programCache.acquireProgram( material, materialProperties.shader, parameters, code );

    materialProperties.program = program;
    material.program = program;

  }

  var programAttributes = program.getAttributes();

  if ( material.morphTargets ) {

    material.numSupportedMorphTargets = 0;

    for ( var i = 0; i < _this.maxMorphTargets; i ++ ) {

      if ( programAttributes[ 'morphTarget' + i ] >= 0 ) {

        material.numSupportedMorphTargets ++;

      }

    }

  }

  if ( material.morphNormals ) {

    material.numSupportedMorphNormals = 0;

    for ( var i = 0; i < _this.maxMorphNormals; i ++ ) {

      if ( programAttributes[ 'morphNormal' + i ] >= 0 ) {

        material.numSupportedMorphNormals ++;

      }

    }

  }

  var uniforms = materialProperties.shader.uniforms;

  if ( ! material.isShaderMaterial &&
       ! material.isRawShaderMaterial ||
       material.clipping === true ) {

    materialProperties.numClippingPlanes = _clipping.numPlanes;
    materialProperties.numIntersection = _clipping.numIntersection;
    uniforms.clippingPlanes = _clipping.uniform;

  }

  materialProperties.fog = fog;

  // store the light setup it was created for

  materialProperties.lightsHash = lights.state.hash;

  if ( material.lights ) {

    // wire up the material to this renderer's lighting state

    uniforms.ambientLightColor.value = lights.state.ambient;
    uniforms.directionalLights.value = lights.state.directional;
    uniforms.spotLights.value = lights.state.spot;
    uniforms.rectAreaLights.value = lights.state.rectArea;
    uniforms.pointLights.value = lights.state.point;
    uniforms.hemisphereLights.value = lights.state.hemi;

    uniforms.directionalShadowMap.value = lights.state.directionalShadowMap;
    uniforms.directionalShadowMatrix.value = lights.state.directionalShadowMatrix;
    uniforms.spotShadowMap.value = lights.state.spotShadowMap;
    uniforms.spotShadowMatrix.value = lights.state.spotShadowMatrix;
    uniforms.pointShadowMap.value = lights.state.pointShadowMap;
    uniforms.pointShadowMatrix.value = lights.state.pointShadowMatrix;
    // TODO (abelnation): add area lights shadow info to uniforms

  }

  var progUniforms = materialProperties.program.getUniforms(),
     uniformsList =
     WebGLUniforms.seqWithValue( progUniforms.seq, uniforms );

  materialProperties.uniformsList = uniformsList;
}

Program::Ptr Renderer_impl::setProgram(Camera::Ptr camera, Fog::Ptr fog, Material::Ptr material, Object3D::Ptr object )
{
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

      p_uniforms->get("logDepthBufFC")->setValue(2.0 / ( log( camera->far() + 1.0 ) / M_LN2 ) );
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
            float size = sqrt( bones.size() * 4 ); // 4 pixels needed for 1 matrix
            size = math::ceilPowerOfTwo( size );
            size = max( size, 4 );

            m.skeleton()->boneMatrices().resize(size * size * 4); // 4 floats per RGBA pixel

            DataTexture::Ptr boneTexture = DataTexture::make(m.skeleton()->boneMatrices(),
                                                             size, size, TextureFormat::RGBA, TextureType::Float );

            m.skeleton()->setBoneTexture(boneTexture);
            m.skeleton()->setBoneTextureSize(size);
          }

          //TODO texture
          //p_uniforms->get("boneTexture")->setValue(m.skeleton()->boneTexture() );
          //p_uniforms->get("boneTextureSize")->setValue(m.skeleton()->boneTextureSize() );

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
    mfuncs.meshStandard = [&] (MeshStandardMaterial &msm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsStandard( m_uniforms, material );
    };
    mfuncs.meshDepth = [&](MeshDepthMaterial &mdm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsDepth( m_uniforms, material );
    };
    mfuncs.meshDistance = [&](MeshDistanceMaterial &mdm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsDistance( m_uniforms, material );
    };
    mfuncs.lineBasic = [&](LineBasicMaterial &lbm) {
      refreshUniformsLine( m_uniforms, material );
    };
    mfuncs.lineDashed = [&](LineDashedMaterial &ldm) {
      refreshUniformsLine( m_uniforms, material );
      refreshUniformsDash( m_uniforms, material );
    };
    /* TODO implement classes
    mfuncs.meshToon = [&](MeshToonMaterial &mtm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsToon( m_uniforms, material );
    };
    mfuncs.meshPysical= [&](MeshPhysicalMaterial &mdm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsPhysical( m_uniforms, material );
    };
    mfuncs.meshNormal = [&](MeshNormalMaterial &mnm) {
      refreshUniformsCommon( m_uniforms, material );
      refreshUniformsNormal( m_uniforms, material );
    };
    mfuncs.points = [&] (PointsMaterial &pm) {
      refreshUniformsPoints( m_uniforms, material );
    };
    mfuncs.shadow = [&] (ShadowMaterial &sm) {
      m_uniforms->color.value = material.color;
      m_uniforms->opacity.value = material.opacity;
    };*/
    material->resolver->call(mfuncs);

    // RectAreaLight Texture
    // TODO (mrdoob): Find a nicer implementation

    if ( m_uniforms.ltcMat !== undefined ) m_uniforms.ltcMat.value = UniformsLib.LTC_MAT_TEXTURE;
    if ( m_uniforms.ltcMag !== undefined ) m_uniforms.ltcMag.value = UniformsLib.LTC_MAG_TEXTURE;

    m_uniforms->upload(this, materialProperties.uniformsList);
  }

  // common matrices
  p_uniforms->get("modelViewMatrix")->setValue(object->modelViewMatrix );
  p_uniforms->get("normalMatrix")->setValue(object->normalMatrix );
  p_uniforms->get("modelMatrix")->setValue(object->matrixWorld() );

  return program;
}

}
}