//
// Created by byter on 29.07.17.
//

#include "Renderer_impl.h"
#include <threepp/helper/Resolver.h>
#include <threepp/math/Math.h>
#include <threepp/textures/DataTexture.h>
#include <sstream>
#include <iostream>
#include <threepp/core/InterleavedBufferAttribute.h>
#include <threepp/objects/Line.h>
#include <threepp/objects/Points.h>
#include <threepp/material/MeshStandardMaterial.h>
#include <threepp/material/MeshPhongMaterial.h>
#include <threepp/material/MeshNormalMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/material/MeshToonMaterial.h>
#include <threepp/material/MeshPhysicalMaterial.h>
#include <threepp/material/PointsMaterial.h>
#include <threepp/material/ShadowMaterial.h>
#include "refresh_uniforms.h"

namespace three {

using namespace std;

OpenGLRenderer::Ptr OpenGLRenderer::make(QOpenGLContext *context, size_t width, size_t height, float pixelRatio,
                                         const OpenGLRendererOptions &options)
{
  gl::Renderer_impl::Ptr p(new gl::Renderer_impl(context, width, height, pixelRatio));

  return p;
}

Renderer::Target::Ptr OpenGLRenderer::makeExternalTarget(GLuint frameBuffer, GLuint texture, size_t width, size_t height,
                                                         CullFace faceCulling, FrontFaceDirection faceDirection,
                                                         bool depthBuffer, bool stencilBuffer)
{
  return gl::RenderTargetExternal::make(frameBuffer, texture, width, height, faceCulling, faceDirection,
                                        depthBuffer, stencilBuffer);
}

Renderer::Target::Ptr OpenGLRenderer::makeInternalTarget(
   size_t width, size_t height, bool depthBuffer, bool stencilBuffer)
{
  return gl::RenderTargetInternal::make(gl::RenderTargetInternal::Options(), width, height);
}

namespace gl {

const std::tuple<size_t, GLuint, bool> Renderer_impl::no_program {0, 0, false};

Renderer_impl::Renderer_impl(QOpenGLContext *context, size_t width, size_t height, float pixelRatio, bool premultipliedAlpha)
   : OpenGLRenderer(context),
     _state(this),
     _width(width),
     _height(height),
     _attributes(this),
     _objects(_geometries, _infoRender),
     _geometries(_attributes),
     _extensions(context),
     _capabilities(this, _extensions, _parameters ),
     _morphTargets(this),
     _shadowMap(*this, _objects, _capabilities),
     _programs(*this, _extensions, _capabilities),
     _premultipliedAlpha(premultipliedAlpha),
     _background(*this, _state, _geometries, premultipliedAlpha),
     _textures(this, _extensions, _state, _properties, _capabilities, _infoMemory),
     _bufferRenderer(this, this, _extensions, _infoRender),
     _indexedBufferRenderer(this, this, _extensions, _infoRender),
     _spriteRenderer(*this, _state, _textures, _capabilities),
     _flareRenderer(this, _state, _textures, _capabilities),
     _pixelRatio(pixelRatio)
{
  initContext();
}

void Renderer_impl::initContext()
{
  initializeOpenGLFunctions();

  _state.init();
  _extensions.get({Extension::ARB_depth_texture, Extension::OES_texture_float,
                   Extension::OES_texture_float_linear, Extension::OES_texture_half_float,
                   Extension::OES_texture_half_float_linear, Extension::OES_standard_derivatives,
                   Extension::ANGLE_instanced_arrays});

  if (_extensions.get(Extension::OES_element_index_uint) ) {

    BufferGeometry::MaxIndex = 4294967296;
  }

  _capabilities.init();

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

Renderer_impl &Renderer_impl::setSize(size_t width, size_t height)
{
  _width = width;
  _height = height;

  setViewport( 0, 0, width, height );
  return *this;
}

Renderer_impl &Renderer_impl::setViewport(size_t x, size_t y, size_t width, size_t height)
{
  _viewport.set( x, _height - y - height, width, height );
  _currentViewport = _viewport * _pixelRatio;
  _state.viewport( _currentViewport );
}

void Renderer_impl::doRender(const Scene::Ptr &scene, const Camera::Ptr &camera,
                             const Renderer::Target::Ptr &renderTarget, bool forceClear)
{
  if(renderTarget) renderTarget->init(this);
  check_glerror(this);

  RenderTarget::Ptr target = dynamic_pointer_cast<RenderTarget>(renderTarget);

  // reset caching for this frame
  _currentGeometryProgram = no_program;
  _currentMaterialId = -1;
  _currentCamera = nullptr;

  // update scene graph
  if (scene->autoUpdate()) scene->updateMatrixWorld(false);

  // update camera matrices and frustum
  if (!camera->parent()) camera->updateMatrixWorld(false);

  _projScreenMatrix.multiply(camera->projectionMatrix(), camera->matrixWorldInverse());
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

  if (_clippingEnabled) _clipping.beginShadows();

  _shadowMap.render(_shadowsArray, scene, camera);

  _lights.setup(_lightsArray, camera);

  if (_clippingEnabled) _clipping.endShadows();

  _infoRender.frame++;
  _infoRender.calls = 0;
  _infoRender.vertices = 0;
  _infoRender.faces = 0;
  _infoRender.points = 0;

  setRenderTarget(target);

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
  if (target)  _textures.updateRenderTargetMipmap(target);

  // Ensure depth buffer writing is enabled so it can be cleared on next render
  _state.depthBuffer.setTest(true);
  _state.depthBuffer.setMask(true);
  _state.colorBuffer.setMask(true);

  state().reset();

  glFinish();
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

std::vector<GLint> Renderer_impl::allocTextureUnits(size_t count)
{
  std::vector<GLint> units(count);
  units.clear();

  for (size_t i = 0; i < count; ++i)
    units.push_back(allocTextureUnit());

  return units;
}

Renderer_impl& Renderer_impl::setRenderTarget(const Renderer::Target::Ptr renderTarget)
{
  _currentRenderTarget = renderTarget;

  RenderTargetCube::Ptr cubeTarget;
  RenderTargetInternal::Ptr internalTarget;
  RenderTargetExternal::Ptr externalTarget;

  GLuint framebuffer = UINT_MAX;

  if(renderTarget) {
    cubeTarget = dynamic_pointer_cast<RenderTargetCube>(renderTarget);
    internalTarget = dynamic_pointer_cast<RenderTargetInternal>(renderTarget);
    externalTarget = dynamic_pointer_cast<RenderTargetExternal>(renderTarget);

    if (cubeTarget) {
      if(cubeTarget->frameBuffers.empty())
        _textures.setupRenderTarget( *cubeTarget );
      framebuffer = cubeTarget->frameBuffers[cubeTarget->activeCubeFace];
    }
    else if(internalTarget) {
      if(!internalTarget->frameBuffer) _textures.setupRenderTarget(*internalTarget);
      framebuffer = internalTarget->frameBuffer;
    }
    else if(externalTarget) {
      if(externalTarget->reuse()) {
        //_textures.setupRenderTarget(*externalTarget);
        framebuffer = externalTarget->frameBuffer;
      }
      else {
        framebuffer = externalTarget->frameBuffer;
        _textures.setDefaultFramebuffer(framebuffer);
      }
    }
    check_glerror(this);

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
    check_glerror(this);
  }

  _state.viewport( _currentViewport );
  _state.scissor( _currentScissor );
  _state.setScissorTest( _currentScissorTest );

  if ( cubeTarget ) {
    auto &textureProperties = _properties.get(renderTarget->texture());
    GLenum textarget = textureProperties.texture;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeTarget->activeCubeFace,
                           textarget, cubeTarget->activeMipMapLevel );
    check_glerror(this);
  }
}

void Renderer_impl::renderObjects(RenderList::iterator renderIterator, Scene::Ptr scene, Camera::Ptr camera, Material::Ptr overrideMaterial)
{
  while(renderIterator) {

    const RenderItem &renderItem = *renderIterator;
    Material::Ptr material = overrideMaterial ? overrideMaterial : renderItem.material;

    camera::Dispatch dispatch;
    dispatch.func<ArrayCamera>() = [&](ArrayCamera &acamera) {
      _currentArrayCamera = dynamic_pointer_cast<ArrayCamera>(camera);

      for ( unsigned j = 0; j < acamera.cameraCount; j ++ ) {

        PerspectiveCamera::Ptr camera2 = acamera[ j ];

        if ( renderItem.object->layers().test( camera2->layers() ) ) {

          /*var bounds = camera2->bounds;

          var x = bounds.x * _width;
          var y = bounds.y * _height;
          var width = bounds.z * _width;
          var height = bounds.w * _height;

          state.viewport( _currentViewport.set( x, y, width, height ).multiplyScalar( _pixelRatio ) );*/

          renderObject( renderItem.object, scene, camera2, renderItem.geometry, material, renderItem.group );
        }
      }
    };
    if(!camera->cameraResolver->getValue(dispatch)) {
      _currentArrayCamera = nullptr;
      renderObject( renderItem.object, scene, camera, renderItem.geometry, material, renderItem.group );
    }
    renderIterator++;
  }
}

void Renderer_impl::renderObject(Object3D::Ptr object, Scene::Ptr scene, Camera::Ptr camera,
                                 BufferGeometry::Ptr geometry, Material::Ptr material, const Group *group)
{
  object->onBeforeRender.emitSignal(*this, scene, camera, geometry, material, group);

  object->modelViewMatrix.multiply(camera->matrixWorldInverse(), object->matrixWorld());
  object->normalMatrix = object->modelViewMatrix.normalMatrix();

  object::Dispatch dispatch;
  dispatch.func<ImmediateRenderObject>() = [&] (ImmediateRenderObject &iro) {
    _state.setMaterial( material, object->frontFaceCW() );

    Program::Ptr program = setProgram( camera, scene->fog(), material, object );

    _currentGeometryProgram = no_program;

    renderObjectImmediate( iro, program, material );
  };
  if(!object->objectResolver->getValue(dispatch)) {
    renderBufferDirect( camera, scene->fog(), geometry, material, object, group );
  }

  object->onAfterRender.emitSignal(*this, scene, camera, geometry, material, group );
}

void Renderer_impl::projectObject(Object3D::Ptr object, Camera::Ptr camera, bool sortObjects )
{
  if (!object->visible()) return;

  bool visible = object->layers().test(camera->layers());
  if (visible && object->objectResolver) {

    object::Dispatch dispatch;

    dispatch.func<Light>() = [&] (Light &light) {
      Light::Ptr lp = dynamic_pointer_cast<Light>(object);
      _lightsArray.push_back(lp);

      if ( light.castShadow ) {
        _shadowsArray.push_back( lp );
      }
    };
    dispatch.func<Sprite>() = [&](Sprite &sprite) {
      Sprite::Ptr sp = dynamic_pointer_cast<Sprite>(object);
      if ( ! sprite.frustumCulled || _frustum.intersectsSprite(*sp) ) {
        _spritesArray.push_back( sp );
      }
    };
    dispatch.func<LensFlare>() = [&](LensFlare &flare) {
      LensFlare::Ptr fp = dynamic_pointer_cast<LensFlare>(object);
      _flaresArray.push_back( fp );
    };
    dispatch.func<ImmediateRenderObject>() = [&](ImmediateRenderObject &iro) {
      if ( sortObjects ) {

        _vector3 = object->matrixWorld().getPosition().apply( _projScreenMatrix );
      }
      _currentRenderList->push_back(object, nullptr, object->material(), _vector3.z(), nullptr );
    };
    resolver::FuncAssoc<Object3D> assoc = [&](Object3D &obj) {

      if ( ! object->frustumCulled || _frustum.intersectsObject( *object ) ) {

        if ( sortObjects ) {
          _vector3 = object->matrixWorld().getPosition().apply( _projScreenMatrix );
        }

        BufferGeometry::Ptr geometry = _objects.update( object );

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
    dispatch.func<SkinnedMesh>() = [&](SkinnedMesh &sk) {
      sk.skeleton()->update();
      assoc(sk);
    };
    dispatch.func<Mesh>() = assoc;
    dispatch.func<Line>() = assoc;
    dispatch.func<LineSegments>() = assoc;
    dispatch.func<Points>() = assoc;

    object->objectResolver->getValue(dispatch);
  }

  for (Object3D::Ptr child : object->children()) {

    projectObject( child, camera, sortObjects );
  }
}

void Renderer_impl::renderObjectImmediate(ImmediateRenderObject &object, Program::Ptr program, Material::Ptr material)
{
  renderBufferImmediate(object, program, material );
}

void Renderer_impl::renderBufferImmediate(ImmediateRenderObject &object, Program::Ptr program, Material::Ptr material)
{
  _state.initAttributes();
#if 0
  //var buffers = _properties.get( object );

  if ( object->hasPositions && ! buffers.position ) buffers.position = _gl.createBuffer();
  if ( object->hasNormals && ! buffers.normal ) buffers.normal = _gl.createBuffer();
  if ( object->hasUvs && ! buffers.uv ) buffers.uv = _gl.createBuffer();
  if ( object->hasColors && ! buffers.color ) buffers.color = _gl.createBuffer();

  var programAttributes = program.getAttributes();

  if ( object.hasPositions ) {

    _gl.bindBuffer( _gl.ARRAY_BUFFER, buffers.position );
    _gl.bufferData( _gl.ARRAY_BUFFER, object.positionArray, _gl.DYNAMIC_DRAW );

    state.enableAttribute( programAttributes.position );
    _gl.vertexAttribPointer( programAttributes.position, 3, _gl.FLOAT, false, 0, 0 );

  }

  if ( object.hasNormals ) {

    _gl.bindBuffer( _gl.ARRAY_BUFFER, buffers.normal );

    if ( ! material.isMeshPhongMaterial &&
         ! material.isMeshStandardMaterial &&
         ! material.isMeshNormalMaterial &&
         material.flatShading === true ) {

      for ( var i = 0, l = object.count * 3; i < l; i += 9 ) {

        var array = object.normalArray;

        var nx = ( array[ i + 0 ] + array[ i + 3 ] + array[ i + 6 ] ) / 3;
        var ny = ( array[ i + 1 ] + array[ i + 4 ] + array[ i + 7 ] ) / 3;
        var nz = ( array[ i + 2 ] + array[ i + 5 ] + array[ i + 8 ] ) / 3;

        array[ i + 0 ] = nx;
        array[ i + 1 ] = ny;
        array[ i + 2 ] = nz;

        array[ i + 3 ] = nx;
        array[ i + 4 ] = ny;
        array[ i + 5 ] = nz;

        array[ i + 6 ] = nx;
        array[ i + 7 ] = ny;
        array[ i + 8 ] = nz;

      }

    }

    _gl.bufferData( _gl.ARRAY_BUFFER, object.normalArray, _gl.DYNAMIC_DRAW );

    state.enableAttribute( programAttributes.normal );

    _gl.vertexAttribPointer( programAttributes.normal, 3, _gl.FLOAT, false, 0, 0 );

  }

  if ( object.hasUvs && material.map ) {

    _gl.bindBuffer( _gl.ARRAY_BUFFER, buffers.uv );
    _gl.bufferData( _gl.ARRAY_BUFFER, object.uvArray, _gl.DYNAMIC_DRAW );

    state.enableAttribute( programAttributes.uv );

    _gl.vertexAttribPointer( programAttributes.uv, 2, _gl.FLOAT, false, 0, 0 );

  }

  if ( object.hasColors && material.vertexColors !== NoColors ) {

    _gl.bindBuffer( _gl.ARRAY_BUFFER, buffers.color );
    _gl.bufferData( _gl.ARRAY_BUFFER, object.colorArray, _gl.DYNAMIC_DRAW );

    state.enableAttribute( programAttributes.color );

    _gl.vertexAttribPointer( programAttributes.color, 3, _gl.FLOAT, false, 0, 0 );

  }

  state.disableUnusedAttributes();

  _gl.drawArrays( _gl.TRIANGLES, 0, object.count );

  object.count = 0;
#endif
}

void Renderer_impl::renderBufferDirect(Camera::Ptr camera,
                                       Fog::Ptr fog,
                                       BufferGeometry::Ptr geometry,
                                       Material::Ptr material,
                                       Object3D::Ptr object,
                                       const Group *group)
{
  _state.setMaterial( material, object->frontFaceCW());

  Program::Ptr program = setProgram( camera, fog, material, object );

  tuple<size_t, GLuint, bool> geometryProgram {geometry->id, program->handle(), material->wireframe};

  bool updateBuffers = false;

  if (geometryProgram != _currentGeometryProgram ) {

    _currentGeometryProgram = geometryProgram;
    updateBuffers = true;
  }

  Mesh::Ptr mesh = dynamic_pointer_cast<Mesh>(object);
  if ( mesh && !mesh->morphTargetInfluences().empty() ) {

    _morphTargets.update( mesh, geometry, material, program );

    updateBuffers = true;
  }

  BufferAttributeT<uint32_t>::Ptr index;
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

  if (index) {

    const Buffer &attribute = _attributes.get( *index );

    if ( updateBuffers )
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attribute.handle);

    _indexedBufferRenderer.setIndex(attribute.type, attribute.bytesPerElement);
    renderer = &_indexedBufferRenderer;
  }
  else {
    renderer = &_bufferRenderer;
  }

  //
  size_t dataCount = 0;

  if (index) {

    dataCount = index->size();
  }
  else if (geometry->position()) {

    dataCount = geometry->position()->itemCount();
  }

  size_t rangeStart = geometry->drawRange().offset * rangeFactor;
  size_t rangeCount = geometry->drawRange().count > 0 ?
                      geometry->drawRange().count * rangeFactor : std::numeric_limits<size_t>::max();

  size_t groupStart = group ? group->start * rangeFactor : 0;
  size_t groupCount = group ? group->count * rangeFactor : numeric_limits<size_t>::max();

  size_t drawStart = std::max( rangeStart, groupStart );
  size_t drawEnd = std::min( dataCount, std::min(rangeStart + rangeCount, groupStart + groupCount)) - 1;

  size_t drawCount = std::max( (size_t)0, drawEnd - drawStart + 1 );

  if ( drawCount == 0 ) return;

  //
  object::Dispatch dispatch;
  dispatch.func<Mesh>() = [&] (Mesh &mesh) {
    if ( material->wireframe ) {

      _state.setLineWidth( material->wireframeLineWidth * getTargetPixelRatio() );
      renderer->setMode(DrawMode::Lines);

    } else {

      renderer->setMode(mesh.drawMode());
    }
  };
  dispatch.func<Line>() = [&] (Line &line) {
    //var lineWidth = material.linewidth;
    //if ( lineWidth === undefined ) lineWidth = 1; // Not using Line*Material

    _state.setLineWidth(line.material<0>()->linewidth * getTargetPixelRatio());

    renderer->setMode(DrawMode::LineStrip);

    //TODO implement classes
    /*if ( object.isLineLoop ) {

      renderer.setMode( _gl.LINE_LOOP );

    }*/
  };
  dispatch.func<LineSegments>() = [&] (LineSegments &line) {

    _state.setLineWidth( line.material<0>()->linewidth * getTargetPixelRatio() );

    renderer->setMode(DrawMode::Lines);
  };
  dispatch.func<Points>() = [&](Points &points) {

    renderer->setMode(DrawMode::Points);
  };
  object->objectResolver->object::DispatchResolver::getValue(dispatch);

  InstancedBufferGeometry::Ptr ibg = dynamic_pointer_cast<InstancedBufferGeometry>(geometry);
  if (ibg) {
    if ( ibg->maxInstancedCount() > 0 ) {
      renderer->renderInstances( ibg, drawStart, drawCount );
    }
  }
  else {

    glValidateProgram(program->handle());
    GLint status;
    glGetProgramiv(program->handle(), GL_VALIDATE_STATUS, &status);
    if(status != GL_TRUE) {
      char buf[500];
      int len;
      glGetProgramInfoLog(program->handle(), 500, &len, buf);
      qWarning() << buf;
    }

    renderer->render( drawStart, drawCount );
  }
}

void Renderer_impl::setupVertexAttributes(Material::Ptr material,
                                          Program::Ptr program,
                                          BufferGeometry::Ptr geometry,
                                          unsigned startIndex)
{
  /*if ( geometry && geometry.isInstancedBufferGeometry ) {
    if ( extensions.get( 'ANGLE_instanced_arrays' ) === null ) {
      console.error( 'THREE.WebGLRenderer.setupVertexAttributes: using THREE.InstancedBufferGeometry but hardware does not support extension ANGLE_instanced_arrays.' );
      return;
    }
  }*/

  _state.initAttributes();

  auto &programAttributes = program->getAttributes();

  for (const auto &att : programAttributes) {

    AttributeName name = att.first;
    GLuint programAttribute = att.second;

    if (programAttribute >= 0) {

      const BufferAttribute::Ptr &geometryAttribute = geometry->getAttribute(name);

      if (geometryAttribute) {

        GLboolean normalized = (GLboolean) geometryAttribute->normalized();
        unsigned size = geometryAttribute->itemSize();

        // TODO Attribute may not be available on context restore

        if (!_attributes.has(*geometryAttribute)) continue;

        Buffer attribute = _attributes.get(*geometryAttribute);

        GLuint buffer = attribute.handle;
        GLenum type = attribute.type;
        unsigned bytesPerElement = attribute.bytesPerElement;

        bufferattribute::Dispatch dispatch;

        dispatch.func<InterleavedBufferAttribute>() = [&](InterleavedBufferAttribute &att) {

          auto &data = att.buffer();
          GLsizei stride = (GLsizei) data.stride();
          GLsizei offset = (GLsizei) att.offset();

          /*if ( data && data.isInstancedInterleavedBuffer ) {

            state.enableAttributeAndDivisor( programAttribute, data.meshPerAttribute );

            if ( geometry.maxInstancedCount === undefined ) {

              geometry.maxInstancedCount = data.meshPerAttribute * data.count;

            }

          } else {*/

          _state.enableAttribute(programAttribute);

          //}

          glBindBuffer(GL_ARRAY_BUFFER, buffer);
          glVertexAttribPointer(programAttribute, size, type, normalized, stride * bytesPerElement,
                                (void *) ((startIndex * stride + offset) * bytesPerElement));
          check_glerror(this);
        };

        if (!geometryAttribute->resolver->bufferattribute::DispatchResolver::getValue(dispatch)) {
          /*if ( geometryAttribute.isInstancedBufferAttribute ) {

            state.enableAttributeAndDivisor( programAttribute, geometryAttribute.meshPerAttribute );

            if ( geometry.maxInstancedCount === undefined ) {

              geometry.maxInstancedCount = geometryAttribute.meshPerAttribute * geometryAttribute.count;

            }

          } else {*/

          _state.enableAttribute(programAttribute);

          //}

          glBindBuffer(GL_ARRAY_BUFFER, buffer);
          glVertexAttribPointer(programAttribute, size, type, normalized, 0, (void *) (startIndex * size * bytesPerElement));
          check_glerror(this);
        }
      }
      else {

        ShaderMaterial::Ptr shaderMat = dynamic_pointer_cast<ShaderMaterial>(material);
        if (shaderMat) {

          switch (name) {

            case AttributeName::color:
              glVertexAttrib3fv(programAttribute, shaderMat->default_color.elements());
              break;
            case AttributeName::uv:
              glVertexAttrib2fv(programAttribute, shaderMat->default_uv.elements());
              break;
            case AttributeName::uv2:
              glVertexAttrib2fv(programAttribute, shaderMat->default_uv2.elements());
              break;
          }
          check_glerror(this);
        }
      }
    }

  }

  _state.disableUnusedAttributes();
}

void Renderer_impl::releaseMaterialProgramReference(Material &material)
{
  auto programInfo = _properties.get( material ).program;

  if (programInfo) {
    _programs.releaseProgram( programInfo );
  }
}

void Renderer_impl::initMaterial(Material::Ptr material, Fog::Ptr fog, Object3D::Ptr object)
{
  static material::ShaderNames shaderNames;

  MaterialProperties &materialProperties = _properties.get( *material );

  ProgramParameters::Ptr parameters = _programs.getParameters(*this,
     material, _lights.state, _shadowsArray, fog, _clipping.numPlanes(), _clipping.numIntersection(), object );

  auto program = materialProperties.program;
  bool programChange = true;

  if (!program) {
    // new material
    material->onDispose.connect([this](Material *material) {
      releaseMaterialProgramReference(*material);
      _properties.remove(*material);
    });
  }
  else if(*program->parameters != *parameters) {
    // changed glsl or parameters
    releaseMaterialProgramReference(*material );
  }
  else if (materialProperties.shaderID != ShaderID::undefined ) {
    // same glsl and uniform list
    return;
  }
  else {
    // only rebuild uniform list
    programChange = false;
  }

  if(programChange) {

    const char *name = material->resolver->material::ShaderNamesResolver::getValue(shaderNames);
    if(*parameters->shaderID != ShaderID::undefined) {

      materialProperties.shader = Shader(name, shaderlib::get(*parameters->shaderID));
    }
    else {
      ShaderMaterial::Ptr sm = dynamic_pointer_cast<ShaderMaterial>(material);
      if(sm)
        materialProperties.shader = Shader(name, sm->uniforms, sm->vertexShader, sm->fragmentShader);
      else
        throw logic_error("unable to determine shader");
    }

    //material.onBeforeCompile( materialProperties.shader );

    program = _programs.acquireProgram( material, materialProperties.shader, parameters);

    materialProperties.program = program;
  }

  const auto &programAttributes = program->getIndexedAttributes();

  if ( material->morphTargets ) {

    material->numSupportedMorphTargets = 0;

    for ( size_t i = 0; i < _maxMorphTargets; i ++ ) {

      IndexedAttributeKey key = make_pair(IndexedAttributeName::morphTarget, i);
      if ( programAttributes.count(key) > 0) {
        material->numSupportedMorphTargets ++;
      }
    }
  }

  if ( material->morphNormals ) {

    material->numSupportedMorphNormals = 0;

    for (size_t i = 0; i < _maxMorphNormals; i ++ ) {

      IndexedAttributeKey key = make_pair(IndexedAttributeName::morphNormal, i);
      if ( programAttributes.count(key) > 0) {
        material->numSupportedMorphNormals ++;
      }
    }
  }

  UniformValues &uniforms = materialProperties.shader.uniforms();

  if( parameters->vertexShader.empty() && parameters->fragmentShader.empty() || parameters->shaderMaterialClipping) {

    materialProperties.numClippingPlanes = _clipping.numPlanes();
    materialProperties.numIntersection = _clipping.numIntersection();
    uniforms.set(UniformName::clippingPlanes, _clipping.uniformValue());
  }

  materialProperties.fog = fog;

  // store the light setup it was created for

  materialProperties.lightsHash = _lights.state.hash;

  if ( material->lights ) {

    // wire up the material to this renderer's lighting state
    uniforms.set(UniformName::spotLights, _lights.state.spot);
    if(!_lights.state.ambient.isNull())
      uniforms.set(UniformName::ambientLightColor, _lights.state.ambient);
    uniforms.set(UniformName::directionalLights, _lights.state.directional);
    uniforms.set(UniformName::hemisphereLights, _lights.state.hemi);
    uniforms.set(UniformName::rectAreaLights, _lights.state.rectArea);
    uniforms.set(UniformName::pointLights, _lights.state.point);

    uniforms.set(UniformName::directionalShadowMap, _lights.state.directionalShadowMap);
    uniforms.set(UniformName::directionalShadowMatrix, _lights.state.directionalShadowMatrix);
    uniforms.set(UniformName::spotShadowMap, _lights.state.spotShadowMap);
    uniforms.set(UniformName::spotShadowMatrix, _lights.state.spotShadowMatrix);
    uniforms.set(UniformName::pointShadowMap, _lights.state.pointShadowMap);
    uniforms.set(UniformName::pointShadowMatrix, _lights.state.pointShadowMatrix);
    // TODO (abelnation): add area lights shadow info to uniforms
  }

  auto progUniforms = materialProperties.program->getUniforms();
  materialProperties.uniformsList = progUniforms->sequenceUniforms(uniforms);
}


void refreshUniforms(UniformValues &uniforms, Fog &fog)
{
  uniforms.set(UniformName::fogColor, fog.color());

  fog::Dispatch dispatch;

  dispatch.func<DefaultFog>() = [&] (DefaultFog &fog) {
    uniforms.set(UniformName::fogNear, fog.near());
    uniforms.set(UniformName::fogFar, fog.far());
  };
  dispatch.func<FogExp2>() = [&] (FogExp2 &fog) {
    uniforms.set(UniformName::fogDensity, fog.density());
  };
  fog.resolver->fog::DispatchResolver::getValue(dispatch);
}

void markUniformsLightsNeedsUpdate(UniformValues &uniforms, bool refreshLights )
{
  uniforms.needsUpdate(UniformName::ambientLightColor, refreshLights);
  uniforms.needsUpdate(UniformName::directionalLights, refreshLights);
  uniforms.needsUpdate(UniformName::pointLights, refreshLights);
  uniforms.needsUpdate(UniformName::spotLights, refreshLights);
  uniforms.needsUpdate(UniformName::rectAreaLights, refreshLights);
  uniforms.needsUpdate(UniformName::hemisphereLights, refreshLights);
}

void uploadUniforms(const std::vector<Uniform::Ptr> &uniformsList, UniformValues &values )
{
  using namespace uniformslib;

  for (auto &up : uniformsList) {

    UniformValue &v = values[up->id()];

    if ( v.needsUpdate ) {

      // note: always updating when .needsUpdate is undefined
      v.applyValue(up);
    }
  }
}

Program::Ptr Renderer_impl::setProgram(Camera::Ptr camera, Fog::Ptr fog, Material::Ptr material, Object3D::Ptr object )
{
  _usedTextureUnits = 0;

  MaterialProperties &materialProperties = _properties.get( material );

  if ( _clippingEnabled ) {

    if ( _localClippingEnabled || camera != _currentCamera ) {

      bool useCache = camera == _currentCamera && material->id == _currentMaterialId;

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

    } else if ( materialProperties.numClippingPlanes > 0 &&
       ( materialProperties.numClippingPlanes != _clipping.numPlanes() ||
          materialProperties.numIntersection != _clipping.numIntersection() ) ) {

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
  UniformValues &m_uniforms = materialProperties.shader.uniforms();

  if (_state.useProgram(program->handle()) ) {

    refreshProgram = true;
    refreshMaterial = true;
    refreshLights = true;
  }

  if ( material->id != _currentMaterialId ) {

    _currentMaterialId = material->id;

    refreshMaterial = true;
  }

  if ( refreshProgram || camera != _currentCamera ) {

    p_uniforms->set(UniformName::projectionMatrix, camera->projectionMatrix());

    if (_capabilities.logarithmicDepthBuffer) {
      PerspectiveCamera::Ptr pcamera = dynamic_pointer_cast<PerspectiveCamera>(camera);
      if(pcamera)
        p_uniforms->set(UniformName::logDepthBufFC, (GLfloat)(2.0 / ( log( pcamera->far() + 1.0 ) / M_LN2 )));
      check_glerror(this);
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

    material::Dispatch dispatch;
    resolver::FuncAssoc<Material> assoc = [&] (Material &mat) {

      if(p_uniforms->get(UniformName::cameraPosition)) {
        _vector3 = camera->matrixWorld().getPosition();
        p_uniforms->set(UniformName::cameraPosition, _vector3);
        check_glerror(this);
      }
    };
    dispatch.func<MeshPhongMaterial>() = assoc;
    dispatch.func<MeshLambertMaterial>() = assoc;
    dispatch.func<MeshBasicMaterial>() = assoc;
    dispatch.func<MeshStandardMaterial>() = assoc;
    dispatch.func<ShaderMaterial>() = assoc;

    material->resolver->material::DispatchResolver::getValue(dispatch);

    assoc = [&] (Material &mat) {

      p_uniforms->set( UniformName::viewMatrix, camera->matrixWorldInverse() );
      check_glerror(this);
    };
    dispatch.func<MeshPhongMaterial>() = assoc;
    dispatch.func<MeshLambertMaterial>() = assoc;
    dispatch.func<MeshBasicMaterial>() = assoc;
    dispatch.func<MeshStandardMaterial>() = assoc;
    dispatch.func<ShaderMaterial>() = assoc;

    if(!material->resolver->material::DispatchResolver::getValue(dispatch) && material->skinning) {
      assoc(*material.get());
    }
  }

  // skinning uniforms must be set even if material didn't change
  // auto-setting of texture unit for bone texture must go before other textures
  // not sure why, but otherwise weird things happen

  if ( material->skinning ) {

    object::Dispatch dispatch;
    dispatch.func<SkinnedMesh>() = [&] (SkinnedMesh &m) {
      p_uniforms->set(UniformName::bindMatrix, m.bindMatrix());
      p_uniforms->set(UniformName::bindMatrixInverse, m.bindMatrixInverse());

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
            size = max( size, 4.0f );

            m.skeleton()->boneMatrices().resize(size * size * 4); // 4 floats per RGBA pixel

            auto ops = DataTexture::options();
            ops.format = TextureFormat::RGBA;
            ops.type = TextureType::Float;
            DataTexture::Ptr boneTexture = nullptr;//DataTexture::make(ops, m.skeleton()->boneMatrices(), size, size);

            m.skeleton()->setBoneTexture(boneTexture);
            m.skeleton()->setBoneTextureSize(size);
          }

          //TODO texture
          //p_uniforms->get("boneTexture")->setValue(m.skeleton()->boneTexture() );
          //p_uniforms->get("boneTextureSize")->setValue(m.skeleton()->boneTextureSize() );

        } else {
          if(!m.skeleton()->boneMatrices().empty())
            p_uniforms->set(UniformName::boneMatrices, m.skeleton()->boneMatrices().data());
        }
      }
    };
    object->objectResolver->getValue(dispatch);
  }
  if ( refreshMaterial ) {

    p_uniforms->set(UniformName::toneMappingExposure, _toneMappingExposure );
    p_uniforms->set(UniformName::toneMappingWhitePoint, _toneMappingWhitePoint );

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

      refreshUniforms( m_uniforms, *fog );
    }

    material::Dispatch dispatch;
    dispatch.func<MeshBasicMaterial>() = [&](MeshBasicMaterial &material) {
      refresh( m_uniforms, material );
    };
    dispatch.func<MeshDepthMaterial>() = [&](MeshDepthMaterial &material) {
      refresh( m_uniforms, material);
    };
    dispatch.func<MeshDistanceMaterial>() = [&](MeshDistanceMaterial &material) {
      refresh( m_uniforms, material);
      m_uniforms[UniformName::referencePosition] = material.referencePosition;
      m_uniforms[UniformName::nearDistance] = material.nearDistance;
      m_uniforms[UniformName::farDistance] = material.farDistance;
    };
    dispatch.func<LineBasicMaterial>() = [&](LineBasicMaterial &material) {
      refresh( m_uniforms, material);
    };
    dispatch.func<LineDashedMaterial>() = [&](LineDashedMaterial &material) {
      refresh( m_uniforms, material );
      m_uniforms[UniformName::dashSize] = material.dashSize;
      m_uniforms[UniformName::totalSize] = material.dashSize + material.gapSize;
      m_uniforms[UniformName::scale] = material.scale;
    };
    dispatch.func<MeshStandardMaterial>() = [&] (MeshStandardMaterial &material) {
      refresh( m_uniforms, material);
    };
    dispatch.func<MeshLambertMaterial>() = [&] (MeshLambertMaterial &material) {
      refresh( m_uniforms, material);
    };
    dispatch.func<MeshPhongMaterial>() = [&](MeshPhongMaterial &material) {
      refresh( m_uniforms, material );
    };
    dispatch.func<MeshToonMaterial>() = [&](MeshToonMaterial &material) {
      refresh( m_uniforms, material );
    };
    dispatch.func<MeshPhysicalMaterial>() = [&](MeshPhysicalMaterial &material) {
      refresh( m_uniforms, material );
      m_uniforms[UniformName::clearCoat] = material.clearCoat;
      m_uniforms[UniformName::clearCoatRoughness] = material.clearCoatRoughness;
    };
    dispatch.func<MeshNormalMaterial>() = [&](MeshNormalMaterial &material) {
      refresh( m_uniforms, material );
    };
    dispatch.func<PointsMaterial>() = [&] (PointsMaterial &material) {
      refresh( m_uniforms, material );
      m_uniforms[UniformName::size] = material.size * _pixelRatio;
      m_uniforms[UniformName::scale] = _height * 0.5f;
    };
    dispatch.func<ShadowMaterial>() = [&] (ShadowMaterial &material) {
      refresh(m_uniforms, material);
    };
    material->resolver->material::DispatchResolver::getValue(dispatch);
    check_glerror(this);

    //in case a material carries ambient color (Assimp)
    if(!material->ambientColor.isNull()) {
      m_uniforms[UniformName::ambientLightColor] = material->ambientColor;
    }

    // RectAreaLight Texture
    // TODO (mrdoob): Find a nicer implementation

    //if ( m_uniforms.ltcMat ) m_uniforms.ltcMat.value = uniforms::LTC_MAT_TEXTURE;
    //if ( m_uniforms.ltcMag ) m_uniforms.ltcMag.value = uniforms::LTC_MAG_TEXTURE;

    uploadUniforms(materialProperties.uniformsList, m_uniforms);
  }

  // common matrices
  p_uniforms->set(UniformName::modelViewMatrix, object->modelViewMatrix );
  p_uniforms->set(UniformName::normalMatrix, object->normalMatrix );
  p_uniforms->set(UniformName::modelMatrix, object->matrixWorld() );

  check_glerror(this);
  return program;
}

void Renderer_impl::setTexture2D(Texture::Ptr texture, GLuint slot)
{
  _textures.setTexture2D(texture, slot);
}

void Renderer_impl::setTextureCube(Texture::Ptr texture, GLuint slot)
{
  _textures.setTextureCube( texture, slot );
}

void RenderTargetExternal::init(Renderer *renderer)
{
  if(!_reuse) {
    Renderer_impl *rimpl = static_cast<Renderer_impl *>(renderer);
    rimpl->_state.setCullFace(_faceCulling);
    rimpl->_state.setFaceDirection(_faceDirection);
    rimpl->_lights.state.clear();
  }
}

}
}
