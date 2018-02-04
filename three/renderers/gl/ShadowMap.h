//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_SHADOWMAP_H
#define THREE_QT_SHADOWMAP_H

#include <three/core/Object3D.h>
#include <three/math/Frustum.h>
#include <three/math/Vector2.h>
#include <three/math/Vector4.h>
#include <three/material/Material.h>
#include <three/material/MeshDepthMaterial.h>
#include <three/material/MeshDistanceMaterial.h>
#include <three/light/Light.h>
#include <three/scene/Scene.h>
#include <three/camera/PerspectiveCamera.h>

#include "Objects.h"
#include "RenderTarget.h"
#include "Capabilities.h"

namespace three {
namespace gl {

class Renderer_impl;

class ShadowMap
{
  math::Frustum _frustum;
  math::Matrix4 _projScreenMatrix;

  math::Vector3 _lookTarget;
  math::Vector3 _lightPositionWorld;

  enum Flag : uint16_t {Morphing = 1, Skinning= 2};

  uint16_t _NumberOfMaterialVariants = (Flag::Morphing | Flag::Skinning) + 1;

  std::vector<Material::Ptr> _depthMaterials;
  std::vector<Material::Ptr> _distanceMaterials;

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

  ShadowMapType _type = ShadowMapType::PCFSoft;

  bool _renderReverseSided = true;
  bool _renderSingleSided = true;

  Renderer_impl &_renderer;
  Objects &_objects;

  const Capabilities &_capabilities;

public:
  ShadowMap(Renderer_impl &renderer, Objects &objects, Capabilities &capabilities)
     : _renderer(renderer), _objects(objects), _capabilities(capabilities)
  {
    for (size_t i = 0; i < _NumberOfMaterialVariants; ++ i ) {

      bool useMorphing = ( i & Flag::Morphing ) != 0;
      bool useSkinning = ( i & Flag::Skinning ) != 0;

      _depthMaterials.push_back(MeshDepthMaterial::make(DepthPacking::RGBA, useMorphing, useSkinning));
      _distanceMaterials.push_back(MeshDistanceMaterial::make(useMorphing, useSkinning));
    }
  }

  void render(std::vector<Light::Ptr> lights, Scene::Ptr scene, Camera::Ptr camera );

  Material::Ptr getDepthMaterial(Object3D::Ptr object,
                            Material::Ptr material,
                            bool isPointLight,
                            const math::Vector3 &lightPositionWorld,
                            float shadowCameraNear,
                            float shadowCameraFar );

  void renderObject(Object3D::Ptr object, Camera::Ptr camera, PerspectiveCamera::Ptr shadowCamera, bool isPointLight);

  bool enabled() const {return _enabled;}

  void setEnabled(bool enabled) {_enabled = enabled;}

  ShadowMapType type() const {return _type;}

  void setType(ShadowMapType type) {_type = type;}
};

}
}


#endif //THREE_QT_SHADOWMAP_H
