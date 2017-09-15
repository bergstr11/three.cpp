//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_SHADOWMAP_H
#define THREE_QT_SHADOWMAP_H

#include <core/Object3D.h>
#include <math/Frustum.h>
#include <math/Vector2.h>
#include <math/Vector4.h>
#include <material/Material.h>
#include <material/MeshDepthMaterial.h>
#include <material/MeshDistanceMaterial.h>
#include "Renderer_impl.h"
#include "RenderTarget.h"

namespace three {
namespace gl {

class ShadowMap
{
  math::Frustum _frustum;
  math::Matrix4 _projScreenMatrix;

  math::Vector2 _shadowMapSize;
  math::Vector2 _maxShadowMapSize;// = new Vector2( maxTextureSize, maxTextureSize ),

  math::Vector3 _lookTarget;
  math::Vector3 _lightPositionWorld;

  enum Flag : uint16_t {Morphing = 1, Skinning= 2};

  uint16_t _NumberOfMaterialVariants = (Flag::Morphing | Flag::Skinning) + 1;

  std::vector<Material::Ptr> _depthMaterials;// = new Array( _NumberOfMaterialVariants ),
  std::vector<Material::Ptr> _distanceMaterials;// = new Array( _NumberOfMaterialVariants ),

  //_materialCache = {};

  math::Vector3 _cubeDirections[] = {
     math::Vector3(1, 0, 0), math::Vector3(-1, 0, 0), math::Vector3(0, 0, 1),
     math::Vector3(0, 0, -1), math::Vector3(0, 1, 0), math::Vector3(0, -1, 0)
  };

  math::Vector3 _cubeUps[] = {
     math::Vector3(0, 1, 0), math::Vector3(0, 1, 0), math::Vector3(0, 1, 0),
     math::Vector3(0, 1, 0), math::Vector3(0, 0, 1), math::Vector3(0, 0, -1)
  };

  math::Vector4 _cube2DViewPorts[6];

  bool _enabled = false;

  bool _autoUpdate = true;
  bool _needsUpdate = false;

  ShadowMapType _type = ShadowMapType::PCF;

  bool _renderReverseSided = true;
  bool _renderSingleSided = true;

  Renderer_impl &_renderer;
  const std::vector<Object3D::Ptr> &_objects;
  const size_t _maxTextureSize;

  // init
  ShadowMap(Renderer_impl &renderer, const std::vector<Object3D::Ptr> &objects, size_t maxTextureSize)
     : _renderer(renderer), _objects(objects), _maxTextureSize(maxTextureSize)
  {
    for (size_t i = 0; i < _NumberOfMaterialVariants; ++ i ) {

      bool useMorphing = ( i & Flag::Morphing ) != 0;
      bool useSkinning = ( i & Flag::Skinning ) != 0;

      _depthMaterials[ i ] = MeshDepthMaterial::make(DepthPacking::RGBA, useMorphing, useSkinning);

      _distanceMaterials[ i ] = MeshDistanceMaterial::make(useMorphing, useSkinning);
    }
  }

  void render(std::vector<Light::Ptr> lights, Scene::Ptr scene, Camera::Ptr camera )
  {

    if (!_enabled) return;
    if (!_autoUpdate && !_needsUpdate) return;

    if ( lights.empty()) return;

    // TODO Clean up (needed in case of contextlost)
    //var _gl = _renderer.context;
    //var _state = _renderer.state;

    // Set GL state for depth map.
    gl::State &state = _renderer.state();
    state.disable( GL_BLEND );
    state.colorBuffer.setClear( 1, 1, 1, 1 );
    state.depthBuffer.setTest( true );
    state.setScissorTest( false );

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

      if ( light->isPointLight() ) {

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
        _cube2DViewPorts[ 0 ].set( vpWidth * 2, vpHeight, vpWidth, vpHeight );
        // negative X
        _cube2DViewPorts[ 1 ].set( 0, vpHeight, vpWidth, vpHeight );
        // positive Z
        _cube2DViewPorts[ 2 ].set( vpWidth * 3, vpHeight, vpWidth, vpHeight );
        // negative Z
        _cube2DViewPorts[ 3 ].set( vpWidth, vpHeight, vpWidth, vpHeight );
        // positive Y
        _cube2DViewPorts[ 4 ].set( vpWidth * 3, 0, vpWidth, vpHeight );
        // negative Y
        _cube2DViewPorts[ 5 ].set( vpWidth, 0, vpWidth, vpHeight );

        _shadowMapSize.x() *= 4.0;
        _shadowMapSize.y() *= 2.0;

      }

      if (!shadow->map() ) {

        RenderTarget::Options pars;
        pars.minFilter = TextureFilter::Nearest;
        pars.magFilter = TextureFilter::Nearest;
        pars.format = TextureFormat::RGBA;

        shadow->setMap(RenderTarget::make( _shadowMapSize.x(), _shadowMapSize.y(), pars ));
        shadow->map().texture().name = light->name() + ".shadowMap";

        shadowCamera->updateProjectionMatrix();
      }

      if ( shadow.isSpotLightShadow ) {

        shadow.update( light );

      }

      var shadowMap = shadow.map;
      var shadowMatrix = shadow.matrix;

      _lightPositionWorld.setFromMatrixPosition( light.matrixWorld );
      shadowCamera.position.copy( _lightPositionWorld );

      if ( light->isPointLight() ) {

        faceCount = 6;

        // for point lights we set the shadow matrix to be a translation-only matrix
        // equal to inverse of the light's position

        shadowMatrix.makeTranslation( - _lightPositionWorld.x, - _lightPositionWorld.y, - _lightPositionWorld.z );

      } else {

        faceCount = 1;

        _lookTarget.setFromMatrixPosition( light.target.matrixWorld );
        shadowCamera.lookAt( _lookTarget );
        shadowCamera.updateMatrixWorld();

        // compute shadow matrix

        shadowMatrix.set(
           0.5, 0.0, 0.0, 0.5,
           0.0, 0.5, 0.0, 0.5,
           0.0, 0.0, 0.5, 0.5,
           0.0, 0.0, 0.0, 1.0
        );

        shadowMatrix.multiply( shadowCamera.projectionMatrix );
        shadowMatrix.multiply( shadowCamera.matrixWorldInverse );

      }

      _renderer.setRenderTarget( shadowMap );
      _renderer.clear();

      // render shadow map for each cube face (if omni-directional) or
      // run a single pass if not

      for ( var face = 0; face < faceCount; face ++ ) {

        if ( light->isPointLight() ) {

          _lookTarget.copy( shadowCamera.position );
          _lookTarget.add( cubeDirections[ face ] );
          shadowCamera.up.copy( cubeUps[ face ] );
          shadowCamera.lookAt( _lookTarget );
          shadowCamera.updateMatrixWorld();

          var vpDimensions = cube2DViewPorts[ face ];
          _state.viewport( vpDimensions );

        }

        // update camera matrices and frustum

        _projScreenMatrix.multiplyMatrices( shadowCamera.projectionMatrix, shadowCamera.matrixWorldInverse );
        _frustum.setFromMatrix( _projScreenMatrix );

        // set object matrices & frustum culling

        renderObject( scene, camera, shadowCamera, light->isPointLight() );

      }

    }

    scope.needsUpdate = false;

  };

  function getDepthMaterial( object, material, isPointLight, lightPositionWorld, shadowCameraNear, shadowCameraFar ) {

    var geometry = object.geometry;

    var result = null;

    var materialVariants = _depthMaterials;
    var customMaterial = object.customDepthMaterial;

    if ( isPointLight ) {

      materialVariants = _distanceMaterials;
      customMaterial = object.customDistanceMaterial;

    }

    if ( ! customMaterial ) {

      var useMorphing = false;

      if ( material.morphTargets ) {

        if ( geometry && geometry.isBufferGeometry ) {

          useMorphing = geometry.morphAttributes && geometry.morphAttributes.position && geometry.morphAttributes.position.length > 0;

        } else if ( geometry && geometry.isGeometry ) {

          useMorphing = geometry.morphTargets && geometry.morphTargets.length > 0;

        }

      }

      if ( object.isSkinnedMesh && material.skinning === false ) {

        console.warn( 'THREE.WebGLShadowMap: THREE.SkinnedMesh with material.skinning set to false:', object );

      }

      var useSkinning = object.isSkinnedMesh && material.skinning;

      var variantIndex = 0;

      if ( useMorphing ) variantIndex |= _MorphingFlag;
      if ( useSkinning ) variantIndex |= _SkinningFlag;

      result = materialVariants[ variantIndex ];

    } else {

      result = customMaterial;

    }

    if ( _renderer.localClippingEnabled &&
         material.clipShadows === true &&
                                  material.clippingPlanes.length !== 0 ) {

      // in this case we need a unique material instance reflecting the
      // appropriate state

      var keyA = result.uuid, keyB = material.uuid;

      var materialsForVariant = _materialCache[ keyA ];

      if ( materialsForVariant === undefined ) {

        materialsForVariant = {};
        _materialCache[ keyA ] = materialsForVariant;

      }

      var cachedMaterial = materialsForVariant[ keyB ];

      if ( cachedMaterial === undefined ) {

        cachedMaterial = result.clone();
        materialsForVariant[ keyB ] = cachedMaterial;

      }

      result = cachedMaterial;

    }

    result.visible = material.visible;
    result.wireframe = material.wireframe;

    var side = material.side;

    if ( scope.renderSingleSided && side == DoubleSide ) {

      side = FrontSide;

    }

    if ( scope.renderReverseSided ) {

      if ( side === FrontSide ) side = BackSide;
      else if ( side === BackSide ) side = FrontSide;

    }

    result.side = side;

    result.clipShadows = material.clipShadows;
    result.clippingPlanes = material.clippingPlanes;
    result.clipIntersection = material.clipIntersection;

    result.wireframeLinewidth = material.wireframeLinewidth;
    result.linewidth = material.linewidth;

    if ( isPointLight && result.isMeshDistanceMaterial ) {

      result.referencePosition.copy( lightPositionWorld );
      result.nearDistance = shadowCameraNear;
      result.farDistance = shadowCameraFar;

    }

    return result;

  }

  function renderObject( object, camera, shadowCamera, isPointLight ) {

    if ( object.visible === false ) return;

    var visible = object.layers.test( camera.layers );

    if ( visible && ( object.isMesh || object.isLine || object.isPoints ) ) {

      if ( object.castShadow && ( ! object.frustumCulled || _frustum.intersectsObject( object ) ) ) {

        object.modelViewMatrix.multiplyMatrices( shadowCamera.matrixWorldInverse, object.matrixWorld );

        var geometry = _objects.update( object );
        var material = object.material;

        if ( Array.isArray( material ) ) {

          var groups = geometry.groups;

          for ( var k = 0, kl = groups.length; k < kl; k ++ ) {

            var group = groups[ k ];
            var groupMaterial = material[ group.materialIndex ];

            if ( groupMaterial && groupMaterial.visible ) {

              var depthMaterial = getDepthMaterial( object, groupMaterial, isPointLight, _lightPositionWorld, shadowCamera.near, shadowCamera.far );
              _renderer.renderBufferDirect( shadowCamera, null, geometry, depthMaterial, object, group );

            }

          }

        } else if ( material.visible ) {

          var depthMaterial = getDepthMaterial( object, material, isPointLight, _lightPositionWorld, shadowCamera.near, shadowCamera.far );
          _renderer.renderBufferDirect( shadowCamera, null, geometry, depthMaterial, object, null );

        }

      }

    }

    var children = object.children;

    for ( var i = 0, l = children.length; i < l; i ++ ) {

      renderObject( children[ i ], camera, shadowCamera, isPointLight );

    }

  }
};

}
}


#endif //THREE_QT_SHADOWMAP_H
