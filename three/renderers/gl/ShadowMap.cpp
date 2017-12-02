//
// Created by byter on 13.09.17.
//

#include "ShadowMap.h"
#include "Renderer_impl.h"
#include <light/PointLight.h>
#include <light/TargetLight.h>

namespace three {
namespace gl {

void ShadowMap::render(std::vector<Light::Ptr> lights, Scene::Ptr scene, Camera::Ptr camera)
{
  if (!_enabled) return;
  if (!_autoUpdate && !_needsUpdate) return;

  if (lights.empty()) return;

  // TODO Clean up (needed in case of contextlost)
  //var _gl = _renderer.context;
  //var _state = _renderer.state;

  // Set GL state for depth map.
  gl::State &state = _renderer.state();
  state.disable(GL_BLEND);
  state.colorBuffer.setClear(1, 1, 1, 1);
  state.depthBuffer.setTest(true);
  state.setScissorTest(false);

  // render depth map
  unsigned faceCount;

  for (Light::Ptr light : lights) {

    auto shadow = light->shadow();

    if (!shadow) {
      //console.warn( 'THREE.WebGLShadowMap:', light, 'has no shadow.' );
      continue;
    }

    const Camera::Ptr shadowCamera = shadow->camera();

    _shadowMapSize = math::min(shadow->mapSize(), _maxShadowMapSize);

    PointLight::Ptr pointLight = std::dynamic_pointer_cast<PointLight>(light);
    if (pointLight) {

      float vpWidth = _shadowMapSize.x();
      float vpHeight = _shadowMapSize.y();

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

      _shadowMapSize.x() *= 4.0;
      _shadowMapSize.y() *= 2.0;
    }

    if (!shadow->map()) {

      RenderTargetDefault::Options pars;
      pars.minFilter = TextureFilter::Nearest;
      pars.magFilter = TextureFilter::Nearest;
      pars.format = TextureFormat::RGBA;

      shadow->map() = RenderTargetDefault::make(pars, _shadowMapSize.x(), _shadowMapSize.y());

      shadowCamera->updateProjectionMatrix();
    }

    shadow->update(light);

    //var shadowMap = shadow.map;
    //var shadowMatrix = shadow.matrix;

    _lightPositionWorld = math::Vector3::fromMatrixPosition(light->matrixWorld());
    shadowCamera->position() = _lightPositionWorld;

    if (pointLight) {

      faceCount = 6;

      // for point lights we set the shadow matrix to be a translation-only matrix
      // equal to inverse of the light's position

      shadow->matrix() = math::Matrix4::translation(-_lightPositionWorld.x(), -_lightPositionWorld.y(), -_lightPositionWorld.z());
    }
    else {
      TargetLight::Ptr targetLight = std::dynamic_pointer_cast<TargetLight>(light);
      assert(targetLight);

      faceCount = 1;

      _lookTarget = math::Vector3::fromMatrixPosition(targetLight->target()->matrixWorld());
      shadowCamera->lookAt(_lookTarget);
      shadowCamera->updateMatrixWorld(true);

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

    _renderer.setRenderTarget(shadow->map());
    _renderer.clear();

    // render shadow map for each cube face (if omni-directional) or
    // run a single pass if not

    for (unsigned face = 0; face < faceCount; face++) {

      if (pointLight) {

        _lookTarget = shadowCamera->position();
        _lookTarget += _cubeDirections[face];
        shadowCamera->up() = _cubeUps[face];
        shadowCamera->lookAt(_lookTarget);
        shadowCamera->updateMatrixWorld(false);

        math::Vector4 &vpDimensions = _cube2DViewPorts[face];
        _renderer.state().viewport(vpDimensions);

      }

      // update camera matrices and frustum
      _projScreenMatrix.multiply(shadowCamera->projectionMatrix(), shadowCamera->matrixWorldInverse());
      _frustum.set(_projScreenMatrix);

      // set object matrices & frustum culling

      if (pointLight)
        renderObject(scene, camera, pointLight->shadow()->specificCamera());
      else
        renderObject(scene, camera, nullptr);
    }
  }

  _needsUpdate = false;
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

  std::vector<Material::Ptr> &materialVariants = _depthMaterials;
  Material::Ptr customMaterial = object->customDepthMaterial;

  if ( isPointLight ) {

    materialVariants = _distanceMaterials;
    customMaterial = object->customDistanceMaterial;
  }

  if ( ! customMaterial ) {

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

    if (_materialCache.find(keyA) == _materialCache.end()) {

      _materialCache.emplace(keyA, std::unordered_map<sole::uuid, Material::Ptr>());
    }
    auto & materialsForVariant = _materialCache[ keyA ];

    if (materialsForVariant.find(keyB) == materialsForVariant.end()) {

      materialsForVariant.emplace(keyB, result);
    }
    result = materialsForVariant[ keyB ];
  }

  result->visible = material->visible;
  result->wireframe = material->wireframe;

  Side side = material->side;

  if ( _renderSingleSided && side == Side::Double ) {

    side = Side::Front;

  }

  if ( _renderReverseSided ) {

    if ( side == Side::Front ) side = Side::Back;
    else if ( side == Side::Back ) side = Side::Front;
  }

  result->side = side;

  result->clipShadows = material->clipShadows;
  result->clippingPlanes = material->clippingPlanes;
  result->clipIntersection = material->clipIntersection;

  result->wireframeLineWidth = material->wireframeLineWidth;

  if (isPointLight) {

    result->setupPointLight(lightPositionWorld, shadowCameraNear, shadowCameraFar);
  }

  return result;
}

void ShadowMap::renderObject(Object3D::Ptr object, Camera::Ptr camera, PerspectiveCamera::Ptr shadowCamera)
{
  if (!object->visible()) return;

  bool isPointLight = (bool)shadowCamera;

  bool visible = object->layers().test( camera->layers() );

  if ( visible && object->renderable()) {

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

      } else {
        Material::Ptr material = object->material();
        if (material->visible) {

          Material::Ptr depthMaterial = getDepthMaterial(object, material, isPointLight, _lightPositionWorld, shadowCamera->near(), shadowCamera->far());
          _renderer.renderBufferDirect(shadowCamera, nullptr, geometry, depthMaterial, object, nullptr);
        }
      }
    }

  }

  std::vector<Object3D::Ptr> children = object->children();

  for (auto child : object->children()) {

    renderObject( child, camera, shadowCamera );
  }
}

}
}