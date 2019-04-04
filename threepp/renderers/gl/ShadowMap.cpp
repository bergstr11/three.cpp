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

  _maxShadowMapSize.set((float)_capabilities.maxTextureSize, (float)_capabilities.maxTextureSize);

  // render depth map
  for (Light::Ptr light : lights) {

    auto &shadow = light->shadow();
    if (!shadow) continue;

    math::Vector2 shadowMapSize = math::min(shadow->mapSize(), _maxShadowMapSize);

    PointLight *pointLight = light->typer;
    if (pointLight) {

      shadowMapSize.x() *= 4.0;
      shadowMapSize.y() *= 2.0;
    }

    const Camera::Ptr shadowCamera = shadow->camera();
    if (!shadow->map()) {

      RenderTargetInternal::Options options;
      options.depthBuffer = false;
      options.stencilBuffer = false;
      options.flipY = true;
      options.minFilter = TextureFilter::Nearest;
      options.magFilter = TextureFilter::Nearest;
      options.format = TextureFormat::RGBA;
      shadow->setMap(RenderTargetInternal::make(options, shadowMapSize.x(), shadowMapSize.y()));

      shadowCamera->updateProjectionMatrix();
    }

    shadow->update();

    const math::Vector3 lightPositionWorld = light->matrixWorld().getPosition();
    shadowCamera->position() = lightPositionWorld;

    if (pointLight) {

      // for point lights we set the shadow matrix to be a translation-only matrix
      // equal to inverse of the light's position

      shadow->matrix() = math::Matrix4::translation(- lightPositionWorld.x(), - lightPositionWorld.y(), - lightPositionWorld.z());
    }
    else {
      TargetLight *targetLight = light->typer;
      assert(targetLight);

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
    unsigned faceCount = 1;

    math::Vector2 shadowMapSize;
    float vpWidth;
    float vpHeight;

    if(pointLight) {

      shadowMapSize = math::min(shadow->mapSize(), _maxShadowMapSize);
      faceCount = 6;

      vpWidth = shadowMapSize.x();
      vpHeight = shadowMapSize.y();
    }

    // render shadow map for each cube face (if omni-directional) or
    // run a single pass if not
    for (unsigned face = 0; face < faceCount; face++) {

      if (pointLight) {
        shadow->camera()->up() = _cubeUps[face];
        shadow->camera()->lookAt(shadow->camera()->position() + _cubeDirections[face]);
        shadow->camera()->updateMatrixWorld(false);

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
        switch(face) {
          case 0:
            // positive X
            _renderer.state().viewport(vpWidth * 2, vpHeight, vpWidth, vpHeight);
            break;
          case 1:
            // negative X
            _renderer.state().viewport(0, vpHeight, vpWidth, vpHeight);
            break;
          case 2:
            // positive Z
            _renderer.state().viewport(vpWidth * 3, vpHeight, vpWidth, vpHeight);
            break;
          case 3:
            // negative Z
            _renderer.state().viewport(vpWidth, vpHeight, vpWidth, vpHeight);
            break;
          case 4:
            // positive Y
            _renderer.state().viewport(vpWidth * 3, 0, vpWidth, vpHeight);
            break;
          case 5:
            // negative Y
            _renderer.state().viewport(vpWidth, 0, vpWidth, vpHeight);
            break;
        }
      }

      // update camera matrices and frustum
      _frustum.set(shadow->camera()->projectionMatrix() * shadow->camera()->matrixWorldInverse());

      // set object matrices & frustum culling
      renderObject(scene, camera, shadow->camera(), pointLight);
      check_glerror(&_renderer);
    }
  }
}

Material::Ptr ShadowMap::getDepthMaterial(Object3D::Ptr object,
                                          Material::Ptr material,
                                          bool isPointLight,
                                          const Camera::Ptr &shadowCamera)
{
  const auto &geometry = object->geometry();
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

  switch(material->side) {
    case three::Side::Back:
      result->side = three::Side::Front;
      break;
    case three::Side::Front:
    case three::Side::Double:
      result->side = three::Side::Back;
      break;
  }

  result->clipShadows = material->clipShadows;
  result->clippingPlanes = material->clippingPlanes;
  result->clipIntersection = material->clipIntersection;

  result->wireframeLineWidth = material->wireframeLineWidth;

  if (isPointLight) {
    if(MeshDistanceMaterial *mat = result->typer) {
      mat->referencePosition = shadowCamera->position();
      mat->nearDistance = shadowCamera->near();
      mat->farDistance = shadowCamera->far();
    }
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

            Material::Ptr depthMaterial = getDepthMaterial(object, groupMaterial, isPointLight, shadowCamera);
            _renderer.renderBufferDirect( shadowCamera, nullptr, geometry, depthMaterial, object, &group );
          }
        }
      }
      else {
        Material::Ptr material = object->material();
        if (material->visible) {
          Material::Ptr depthMaterial = getDepthMaterial(object, material, isPointLight, shadowCamera);

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
