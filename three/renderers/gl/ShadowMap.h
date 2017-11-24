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
#include <light/Light.h>
#include <scene/Scene.h>
#include <camera/PerspectiveCamera.h>

#include "Objects.h"
#include "RenderTarget.h"

namespace three {
namespace gl {

class Renderer_impl;

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

  std::unordered_map<sole::uuid, std::unordered_map<sole::uuid, Material::Ptr>> _materialCache;

  math::Vector3 _cubeDirections[6] {
     math::Vector3(1, 0, 0), math::Vector3(-1, 0, 0), math::Vector3(0, 0, 1),
     math::Vector3(0, 0, -1), math::Vector3(0, 1, 0), math::Vector3(0, -1, 0)
  };

  math::Vector3 _cubeUps[6] {
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
  Objects &_objects;
  const size_t _maxTextureSize;

public:
  ShadowMap(Renderer_impl &renderer, Objects &objects, size_t maxTextureSize)
     : _renderer(renderer), _objects(objects), _maxTextureSize(maxTextureSize)
  {
    _depthMaterials.resize(_NumberOfMaterialVariants);
    _distanceMaterials.resize(_NumberOfMaterialVariants);

    for (size_t i = 0; i < _NumberOfMaterialVariants; ++ i ) {

      bool useMorphing = ( i & Flag::Morphing ) != 0;
      bool useSkinning = ( i & Flag::Skinning ) != 0;

      _depthMaterials[ i ] = MeshDepthMaterial::make(DepthPacking::RGBA, useMorphing, useSkinning);
      _distanceMaterials[ i ] = MeshDistanceMaterial::make(useMorphing, useSkinning);
    }
  }

  void render(std::vector<Light::Ptr> lights, Scene::Ptr scene, Camera::Ptr camera );

  Material::Ptr getDepthMaterial(Object3D::Ptr object,
                            Material::Ptr material,
                            bool isPointLight,
                            const math::Vector3 &lightPositionWorld,
                            float shadowCameraNear,
                            float shadowCameraFar );

  void renderObject(Object3D::Ptr object, Camera::Ptr camera, PerspectiveCamera::Ptr shadowCamera);

  bool enabled() const {return _enabled;}

  ShadowMapType type() const {return _type;}
};

}
}


#endif //THREE_QT_SHADOWMAP_H
