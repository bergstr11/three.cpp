//
// Created by byter on 25.10.17.
//

#include "Lights.h"

namespace three {
namespace gl {

using namespace std;

void Lights::setup(const vector<Light::Ptr> &lights, unsigned numShadows, Camera::Ptr camera )
{
  Color ambient {0, 0, 0};

  const math::Matrix4 &viewMatrix = camera->matrixWorldInverse();

  for (Light::Ptr light : lights) {

    const Color &color = light->color();
    float intensity = light->intensity();

    Texture::Ptr shadowMap = light->shadow() && light->shadow()->map() ? light->shadow()->map()->texture() : nullptr;

    if(AmbientLight *alight = light->typer) {
      ambient += color * intensity;
    }
    else if(DirectionalLight *dlight = light->typer) {
      lights::Entry<DirectionalLight>::Ptr uniforms = cache.get( dlight );

      uniforms->color = light->color() * light->intensity();

      uniforms->direction = light->matrixWorld().getPosition();
      uniforms->direction -= dlight->target()->matrixWorld().getPosition();
      uniforms->direction.transformDirection( viewMatrix );

      uniforms->shadow = light->castShadow;

      if (light->castShadow) {

        uniforms->shadowBias = light->shadow()->bias();
        uniforms->shadowRadius = light->shadow()->radius();
        uniforms->shadowMapSize = light->shadow()->mapSize();
      }

      if(shadowMap) {
        state.directionalShadowMap.push_back(shadowMap);
        state.directionalShadowMatrix.push_back(light->shadow()->matrix());
      }
      state.directional.push_back(uniforms);
    }
    else if(SpotLight *slight = light->typer) {
      lights::Entry<SpotLight>::Ptr uniforms = cache.get( slight );

      uniforms->position = light->matrixWorld().getPosition();
      uniforms->position.apply( viewMatrix );

      uniforms->color = color * intensity;
      uniforms->distance = slight->distance();

      uniforms->direction = light->matrixWorld().getPosition();
      uniforms->direction -= slight->target()->matrixWorld().getPosition();
      uniforms->direction.transformDirection( viewMatrix );

      uniforms->coneCos = cos( slight->angle() );
      uniforms->penumbraCos = cos( slight->angle() * ( 1 - slight->penumbra() ) );
      uniforms->decay = ( slight->distance() == 0 ) ? 0.0f : slight->decay();

      uniforms->shadow = light->castShadow;

      if (light->castShadow) {

        LightShadow::Ptr shadow = light->shadow();

        uniforms->shadowBias = shadow->bias();
        uniforms->shadowRadius = shadow->radius();
        uniforms->shadowMapSize = shadow->mapSize();
      }

      state.spotShadowMap.push_back(shadowMap);
      state.spotShadowMatrix.push_back(light->shadow()->matrix());
      state.spot.push_back(uniforms);
    }
    else if(RectAreaLight *rlight = light->typer) {
      lights::Entry<RectAreaLight>::Ptr uniforms = cache.get( rlight );

      // (a) intensity controls irradiance of entire light
      uniforms->color = color  * ( intensity / ( rlight->width() * rlight->height()));

      // (b) intensity controls the radiance per light area
      // uniforms.color.copy( color ).multiplyScalar( intensity );

      uniforms->position = light->matrixWorld().getPosition();
      uniforms->position.apply( viewMatrix );

      // extract local rotation of light to derive width/height half vectors
      math::Matrix4 matrix4 = light->matrixWorld();
      matrix4.premultiply( viewMatrix );
      math::Matrix4 matrix42 = math::Matrix4 ::rotation( matrix4 );

      uniforms->halfWidth = math::Vector3(rlight->width() * 0.5, 0.0, 0.0 );
      uniforms->halfHeight = math::Vector3(0.0, rlight->height() * 0.5, 0.0 );

      uniforms->halfWidth.apply( matrix42 );
      uniforms->halfHeight.apply( matrix42 );

      // TODO (abelnation): RectAreaLight distance?
      // uniforms.distance = distance;

      state.rectArea.push_back(uniforms);
    }
    else if(PointLight *plight = light->typer) {
      lights::Entry<PointLight>::Ptr uniforms = cache.get( plight );

      uniforms->position = light->matrixWorld().getPosition();
      uniforms->position.apply( viewMatrix );

      uniforms->color = light->color() * light->intensity();
      uniforms->distance = plight->distance();
      uniforms->decay = (plight->distance() == 0) ? 0.0f : plight->decay();

      uniforms->shadow = light->castShadow;

      if (light->castShadow) {

        auto shadow = plight->shadow();

        uniforms->shadowBias = shadow->bias();
        uniforms->shadowRadius = shadow->radius();
        uniforms->shadowMapSize = shadow->mapSize();
        uniforms->shadowCameraNear = shadow->camera()->near();
        uniforms->shadowCameraFar = shadow->camera()->far();
      }

      state.pointShadowMap.push_back(shadowMap);
      state.pointShadowMatrix.push_back(plight->shadow()->matrix());
      state.point.push_back(uniforms);
    }
    else if(HemisphereLight *hlight = light->typer) {
      lights::Entry<HemisphereLight>::Ptr uniforms = cache.get( hlight );

      uniforms->direction = light->matrixWorld().getPosition();
      uniforms->direction.transformDirection( viewMatrix );
      uniforms->direction.normalize();

      uniforms->skyColor = light->color() * intensity;
      uniforms->groundColor = hlight->groundColor() * intensity;

      state.hemi.push_back(uniforms);
    }
  }

  state.ambient = ambient;
  state.storeHash(numShadows);
}

}
}