//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_LIGHTS_H
#define THREE_QT_LIGHTS_H

#include <light/AmbientLight.h>
#include <light/DirectionalLight.h>
#include <light/HemisphereLight.h>
#include <light/PointLight.h>
#include <light/SpotLight.h>
#include <light/RectAreaLight.h>
#include <light/LightShadow.h>
#include <shadow.h>
#include <unordered_map>
#include "../Resolver.h"

namespace three {
namespace gl {

struct UniformsBase
{
  virtual void apply() {}
  using Ptr = std::shared_ptr<UniformsBase>;
};

template<typename T>
struct Uniforms {};

template<>
struct Uniforms<DirectionalLight> : public UniformsBase
{
  using Ptr = std::shared_ptr<Uniforms<DirectionalLight>>;

  math::Vector3 direction;
  Color color;
  bool shadow = false;
  float shadowBias = 0;
  float shadowRadius = 1;
  math::Vector2 shadowMapSize;
};

template<>
struct Uniforms<SpotLight> : public UniformsBase
{
  using Ptr = std::shared_ptr<Uniforms<SpotLight>>;

  math::Vector3 position;
  math::Vector3 direction;
  Color color;
  float distance = 0;
  float coneCos = 0;
  float penumbraCos = 0;
  float decay = 0;
  bool shadow = false;
  float shadowBias = 0;
  float shadowRadius = 1;
  math::Vector2 shadowMapSize;
};

template<>
struct Uniforms<PointLight> : public UniformsBase
{
  using Ptr = std::shared_ptr<Uniforms<PointLight>>;

  math::Vector3 position;
  Color color;
  float distance = 0;
  float decay = 0;
  bool shadow = false;
  float shadowBias = 0;
  float shadowRadius = 1;
  math::Vector2 shadowMapSize;
  float shadowCameraNear = 1;
  float shadowCameraFar = 1000;
};

template<>
struct Uniforms<HemisphereLight> : public UniformsBase
{
  using Ptr = std::shared_ptr<Uniforms<HemisphereLight>>;

  math::Vector3 direction;
  Color skyColor;
  Color groundColor;
};

template<>
struct Uniforms<RectAreaLight> : public UniformsBase
{
  using Ptr = std::shared_ptr<Uniforms<RectAreaLight>>;

  math::Vector3 position;
  Color color;
  math::Vector3 halfWidth;
  math::Vector3 halfHeight;
};

class UniformsCache
{
  std::unordered_map<unsigned, UniformsBase::Ptr> lights;

public:

  template <typename L>
  const std::shared_ptr<Uniforms<L>> get(std::shared_ptr<L> light)
  {
    if (lights.find(light->id) != lights.end()) {

      return std::dynamic_pointer_cast<Uniforms<L>>(lights[ light->id ]);
    }

    std::shared_ptr<Uniforms<L>> u = std::make_shared<Uniforms<L>>();
    lights[light->id()] = u;

    return u;
  }
};

class Lights
{
  UniformsCache cache;

  struct State {
    std::vector<Texture::Ptr> directionalShadowMap;
    std::vector<math::Matrix4 &> directionalShadowMatrix;
    std::vector<UniformsBase::Ptr> directional;
     /*ambient: [ 0, 0, 0 ],
     spot: [],
     spotShadowMap: [],
     spotShadowMatrix: [],
     rectArea: [],
     point: [],
     pointShadowMap: [],
     pointShadowMatrix: [],
     hemi: []*/
  } state;

  //var vector3 = new Vector3();
  //var matrix4 = new Matrix4();
  //var matrix42 = new Matrix4();

  void setup(std::vector<Light::Ptr> lights, std::vector<LightShadow::Ptr> shadows, Camera::Ptr camera )
  {
    float r = 0, g = 0, b = 0;

    /*var directionalLength = 0;
    var pointLength = 0;
    var spotLength = 0;
    var rectAreaLength = 0;
    var hemiLength = 0;*/

    const math::Matrix4 &viewMatrix = camera->matrixWorldInverse();

    for (Light::Ptr light : lights) {

      const Color &color = light->color();
      float intensity = light->intensity();
      //var distance = light.distance;

      Texture::Ptr shadowMap = ( light->shadow() && light->shadow()->map() ) ?
                               light->shadow()->map()->texture() : nullptr;

      LightFunctions lightFuncs;

      lightFuncs.ambient = [&](AmbientLight::Ptr alight)
      {
        r += color.r * intensity;
        g += color.g * intensity;
        b += color.b * intensity;
      };
      lightFuncs.directional = [&](DirectionalLight::Ptr dlight)
      {
        Uniforms<DirectionalLight>::Ptr uniforms = cache.get( light );

        uniforms->color = light->color() * light->intensity();

        uniforms->direction = math::Vector3::fromMatrixPosition(light->matrixWorld());
        uniforms->direction -= math::Vector3::fromMatrixPosition(dlight->target()->matrixWorld());
        uniforms->direction.transformDirection( viewMatrix );

        uniforms->shadow = light->castShadow();

        if (light->castShadow()) {

          uniforms->shadowBias = light->shadow()->bias();
          uniforms->shadowRadius = light->shadow()->radius();
          uniforms->shadowMapSize = light->shadow()->mapSize();
        }

        state.directionalShadowMap.push_back(shadowMap);
        state.directionalShadowMatrix.push_back(light->shadow()->matrix());
        state.directional.push_back(uniforms);
      };
      lightFuncs.spot = [&](SpotLight::Ptr slight)
      {
        Uniforms<SpotLight>::Ptr uniforms = cache.get( light );

        uniforms->position = math::Vector3::fromMatrixPosition(light->matrixWorld());
        uniforms->position.apply( viewMatrix );

        uniforms->color = color * intensity;
        uniforms->distance = light->distance();

        uniforms->direction = math::Vector3::fromMatrixPosition( light->matrixWorld() );
        uniforms->direction -= math::Vector3::fromMatrixPosition( slight->target()->matrixWorld());
        uniforms->direction.transformDirection( viewMatrix );

        uniforms->coneCos = std::cos( light->angle() );
        uniforms.penumbraCos = Math.cos( light.angle * ( 1 - light.penumbra ) );
        uniforms.decay = ( light.distance === 0 ) ? 0.0 : light.decay;

        uniforms.shadow = light.castShadow;

        if ( light.castShadow ) {

          var shadow = light.shadow;

          uniforms.shadowBias = shadow.bias;
          uniforms.shadowRadius = shadow.radius;
          uniforms.shadowMapSize = shadow.mapSize;

        }

        state.spotShadowMap[ spotLength ] = shadowMap;
        state.spotShadowMatrix[ spotLength ] = light.shadow.matrix;
        state.spot[ spotLength ] = uniforms;

        spotLength ++;

      } else if ( light.isRectAreaLight ) {

        var uniforms = cache.get( light );

        // (a) intensity controls irradiance of entire light
        uniforms.color
           .copy( color )
           .multiplyScalar( intensity / ( light.width * light.height ) );

        // (b) intensity controls the radiance per light area
        // uniforms.color.copy( color ).multiplyScalar( intensity );

        uniforms.position.setFromMatrixPosition( light.matrixWorld );
        uniforms.position.applyMatrix4( viewMatrix );

        // extract local rotation of light to derive width/height half vectors
        matrix42.identity();
        matrix4.copy( light.matrixWorld );
        matrix4.premultiply( viewMatrix );
        matrix42.extractRotation( matrix4 );

        uniforms.halfWidth.set( light.width * 0.5,                0.0, 0.0 );
        uniforms.halfHeight.set(              0.0, light.height * 0.5, 0.0 );

        uniforms.halfWidth.applyMatrix4( matrix42 );
        uniforms.halfHeight.applyMatrix4( matrix42 );

        // TODO (abelnation): RectAreaLight distance?
        // uniforms.distance = distance;

        state.rectArea[ rectAreaLength ] = uniforms;

        rectAreaLength ++;

      } else if ( light.isPointLight ) {

        var uniforms = cache.get( light );

        uniforms.position.setFromMatrixPosition( light.matrixWorld );
        uniforms.position.applyMatrix4( viewMatrix );

        uniforms.color.copy( light.color ).multiplyScalar( light.intensity );
        uniforms.distance = light.distance;
        uniforms.decay = ( light.distance === 0 ) ? 0.0 : light.decay;

        uniforms.shadow = light.castShadow;

        if ( light.castShadow ) {

          var shadow = light.shadow;

          uniforms.shadowBias = shadow.bias;
          uniforms.shadowRadius = shadow.radius;
          uniforms.shadowMapSize = shadow.mapSize;
          uniforms.shadowCameraNear = shadow.camera.near;
          uniforms.shadowCameraFar = shadow.camera.far;

        }

        state.pointShadowMap[ pointLength ] = shadowMap;
        state.pointShadowMatrix[ pointLength ] = light.shadow.matrix;
        state.point[ pointLength ] = uniforms;

        pointLength ++;

      } else if ( light.isHemisphereLight ) {

        var uniforms = cache.get( light );

        uniforms.direction.setFromMatrixPosition( light.matrixWorld );
        uniforms.direction.transformDirection( viewMatrix );
        uniforms.direction.normalize();

        uniforms.skyColor.copy( light.color ).multiplyScalar( intensity );
        uniforms.groundColor.copy( light.groundColor ).multiplyScalar( intensity );

        state.hemi[ hemiLength ] = uniforms;

        hemiLength ++;

      }

    }

    state.ambient[ 0 ] = r;
    state.ambient[ 1 ] = g;
    state.ambient[ 2 ] = b;

    state.directional.length = directionalLength;
    state.spot.length = spotLength;
    state.rectArea.length = rectAreaLength;
    state.point.length = pointLength;
    state.hemi.length = hemiLength;

    // TODO (sam-g-steel) why aren't we using join
    state.hash = directionalLength + ',' + pointLength + ',' + spotLength + ',' + rectAreaLength + ',' + hemiLength + ',' + shadows.length;

  }

  return {
     setup: setup,
     state: state
  }

}

}
}
#endif //THREE_QT_LIGHTS_H
