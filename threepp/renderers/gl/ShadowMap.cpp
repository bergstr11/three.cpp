//
// Created by byter on 13.09.17.
//

#include "ShadowMap.h"
#include "Renderer_impl.h"

namespace three {
namespace gl {

ShadowMap::ShadowMap(Renderer_impl &renderer, Objects &objects, Capabilities &capabilities)
: _renderer(renderer), _objects(objects), _capabilities(capabilities)
{
  static constexpr uint16_t _NumberOfMaterialVariants = (Flag::Morphing | Flag::Skinning) + 1;

  for (size_t i = 0; i < _NumberOfMaterialVariants; ++ i ) {

    bool useMorphing = ( i & Flag::Morphing ) != 0;
    bool useSkinning = ( i & Flag::Skinning ) != 0;

    _depthMaterials.push_back(MeshDepthMaterial::make(DepthPacking::RGBA, useMorphing, useSkinning));
    _distanceMaterials.push_back(MeshDistanceMaterial::make(useMorphing, useSkinning));
  }
}

void ShadowMap::setup(std::vector<Light::Ptr> lights, Scene::Ptr scene, Camera::Ptr camera)
{
  if (!enabled || lights.empty() || (!autoUpdate && !needsUpdate)) {
    _needsRender = false;
    return;
  }
  _needsRender = true;

  // render depth map
  for (Light::Ptr light : lights) {

    auto shadow = light->shadow();
    if (!shadow) continue;

    math::Vector2 maxShadowMapSize {(float)_capabilities.maxTextureSize, (float)_capabilities.maxTextureSize};
    math::Vector2 shadowMapSize = math::min(shadow->mapSize(), maxShadowMapSize);

    PointLight *pointLight = light->typer;
    if (pointLight) {

      float vpWidth = shadowMapSize.x();
      float vpHeight = shadowMapSize.y();

      // These viewports map a cube-map onto a 2D texture with the
      // following orientation:
      //
      //  xzXZ
      //   y Y
      //
      // X - Positive x direction
      // x - Negative x direction
      // Y - Positive y direction
      // y - Negative y direction
      // Z - Positive z direction
      // z - Negative z direction

      // positive X
      _cube2DViewPorts[0].set(vpWidth * 2, vpHeight, vpWidth, vpHeight);
      // negative X
      _cube2DViewPorts[1].set(0, vpHeight, vpWidth, vpHeight);
      // positive Z
      _cube2DViewPorts[2].set(vpWidth * 3, vpHeight, vpWidth, vpHeight);
      // negative Z
      _cube2DViewPorts[3].set(vpWidth, vpHeight, vpWidth, vpHeight);
      // positive Y
      _cube2DViewPorts[4].set(vpWidth * 3, 0, vpWidth, vpHeight);
      // negative Y
      _cube2DViewPorts[5].set(vpWidth, 0, vpWidth, vpHeight);

      shadowMapSize.x() *= 4.0;
      shadowMapSize.y() *= 2.0;
    }

    const Camera::Ptr shadowCamera = shadow->camera();
    if (!shadow->map()) {

      RenderTargetInternal::Options options;
      options.minFilter = TextureFilter::Nearest;
      options.magFilter = TextureFilter::Nearest;
      options.format = TextureFormat::RGBA;
      shadow->setMap(RenderTargetInternal::make(options, shadowMapSize.width(), shadowMapSize.height()));

      shadowCamera->updateProjectionMatrix();
    }

    shadow->update();

    _lightPositionWorld = light->matrixWorld().getPosition();
    shadowCamera->position() = _lightPositionWorld;

    if (pointLight) {

      _faceCount = 6;

      // for point lights we set the shadow matrix to be a translation-only matrix
      // equal to inverse of the light's position

      shadow->matrix() = math::Matrix4::translation(-_lightPositionWorld.x(), -_lightPositionWorld.y(), -_lightPositionWorld.z());
    }
    else {
      TargetLight *targetLight = light->typer;
      assert(targetLight);

      _faceCount = 1;

      math::Vector3 lookTarget = targetLight->target()->matrixWorld().getPosition();
      shadowCamera->lookAt(lookTarget);
      shadowCamera->updateMatrixWorld(false);

      // compute shadow matrix
      shadow->matrix() = math::Matrix4(
         0.5, 0.0, 0.0, 0.5,
         0.0, 0.5, 0.0, 0.5,
         0.0, 0.0, 0.5, 0.5,
         0.0, 0.0, 0.0, 1.0
      );
      shadow->matrix() *= shadowCamera->projectionMatrix();
      shadow->matrix() *= shadowCamera->matrixWorldInverse();
    }

    // render shadow map for each cube face (if omni-directional) or
    // run a single pass if not
    if (pointLight) {

      for (unsigned face = 0; face < _faceCount; face++) {

          math::Vector3 lookTarget = shadowCamera->position();
          lookTarget += _cubeDirections[face];
          shadowCamera->up() = _cubeUps[face];
          shadowCamera->lookAt(lookTarget);
          shadowCamera->updateMatrixWorld(false);
      }
    }
  }

  needsUpdate = false;
}

void ShadowMap::render(std::vector<Light::Ptr> lights, Scene::Ptr scene, Camera::Ptr camera)
{
  if(!_needsRender) return;

  // Set GL state for depth map.
  gl::State &state = _renderer.state();
  state.disable(GL_BLEND);
  state.colorBuffer.setClear(1, 1, 1, 1);
  state.depthBuffer.setTest(true);
  state.setScissorTest(false);

  check_glerror(&_renderer);

  // render depth map
  for (Light::Ptr light : lights) {

    auto shadow = light->shadow();
    if (!shadow || !shadow->map()) continue;

    _renderer.setRenderTarget(shadow->map());
    _renderer.clear(true, true, true);

    bool pointLight = light->is<PointLight>();

    // render shadow map for each cube face (if omni-directional) or
    // run a single pass if not
    for (unsigned face = 0; face < _faceCount; face++) {

      if (pointLight) {

        math::Vector4 &vpDimensions = _cube2DViewPorts[face];
        _renderer.state().viewport(vpDimensions);
      }

      // update camera matrices and frustum
      _frustum.set(shadow->camera()->projectionMatrix() * shadow->camera()->matrixWorldInverse());

      // set object matrices & frustum culling
      renderObject(scene, camera, shadow->camera(), (bool)pointLight);
      check_glerror(&_renderer);
    }
  }
}

Material::Ptr ShadowMap::getDepthMaterial(Object3D::Ptr object,
                               Material::Ptr material,
                               bool isPointLight,
                               const math::Vector3 &lightPositionWorld,
                               float shadowCameraNear,
                               float shadowCameraFar )
{
  auto geometry = object->geometry();
  Material::Ptr result;

  std::vector<Material::Ptr> &materialVariants = isPointLight ? _distanceMaterials : _depthMaterials;
  Material::Ptr customMaterial = isPointLight ? object->customDistanceMaterial : object->customDepthMaterial;

  if (!customMaterial) {

    bool useMorphing = material->morphTargets ? geometry->useMorphing() : false;

    bool useSkinning = object->skinned() && material->skinning;

    unsigned variantIndex = 0;

    if ( useMorphing ) variantIndex |= Flag::Morphing;
    if ( useSkinning ) variantIndex |= Flag::Skinning;

    result = materialVariants[ variantIndex ];
  }
  else {

    result = customMaterial;
  }

  if ( _renderer.localClippingEnabled() &&
       material->clipShadows && !material->clippingPlanes.empty()) {

    // in this case we need a unique material instance reflecting the
    // appropriate state

    const sole::uuid &keyA = result->uuid;
    const sole::uuid &keyB = material->uuid;

    if (!_materialCache.count(keyA)) {

      _materialCache.emplace(keyA, std::unordered_map<sole::uuid, Material::Ptr>());
    }
    auto & materialsForVariant = _materialCache[ keyA ];

    if (!materialsForVariant.count(keyB)) {

      materialsForVariant.emplace(keyB, result);
    }
    result = materialsForVariant[ keyB ];
  }

  result->visible = material->visible;
  result->wireframe = material->wireframe;

  result->side = material->side;

  result->clipShadows = material->clipShadows;
  result->clippingPlanes = material->clippingPlanes;
  result->clipIntersection = material->clipIntersection;

  result->wireframeLineWidth = material->wireframeLineWidth;

  if (isPointLight) {

    result->setupPointLight(lightPositionWorld, shadowCameraNear, shadowCameraFar);
  }

  return result;
}

void ShadowMap::renderObject(Object3D::Ptr object, Camera::Ptr camera, Camera::Ptr shadowCamera, bool isPointLight)
{
  if (!object->visible()) return;

  bool visible = object->layers().test( camera->layers() );

  if ( visible && object->isShadowRenderable()) {

    if ( object->castShadow && ( ! object->frustumCulled || _frustum.intersectsObject( *object ) ) ) {

      object->modelViewMatrix.multiply(shadowCamera->matrixWorldInverse(), object->matrixWorld());
      BufferGeometry::Ptr geometry = _objects.update( object );

      if ( object->materialCount() > 1 ) {

        const std::vector<Group> &groups = geometry->groups();

        for (const Group &group : groups) {

          Material::Ptr groupMaterial = object->material(group.materialIndex);

          if ( groupMaterial && groupMaterial->visible ) {

            Material::Ptr depthMaterial = getDepthMaterial(object, groupMaterial, isPointLight, _lightPositionWorld,
                                                 shadowCamera->near(), shadowCamera->far() );
            _renderer.renderBufferDirect( shadowCamera, nullptr, geometry, depthMaterial, object, &group );
          }
        }
      }
      else {
        Material::Ptr material = object->material();
        if (material->visible) {
          Material::Ptr depthMaterial = getDepthMaterial(object, material, isPointLight, _lightPositionWorld,
                                                         shadowCamera->near(), shadowCamera->far());

          _renderer.renderBufferDirect(shadowCamera, nullptr, geometry, depthMaterial, object, nullptr);
        }
      }
    }
  }

  std::vector<Object3D::Ptr> children = object->children();

  for (auto child : object->children()) {

    renderObject( child, camera, shadowCamera, isPointLight);
  }
}

}
}
