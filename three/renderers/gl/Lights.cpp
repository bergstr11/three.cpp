//
// Created by byter on 25.10.17.
//

#include "Lights.h"
#include <sstream>

namespace three {
namespace gl {

using namespace std;

void Lights::setup(const vector<Light::Ptr> &lights, Camera::Ptr camera )
{
  float r = 0, g = 0, b = 0;

  state.directional.clear();
  state.point.clear();
  state.spot.clear();
  state.rectArea.clear();
  state.hemi.clear();

  const math::Matrix4 &viewMatrix = camera->matrixWorldInverse();

  for (Light::Ptr light : lights) {

    const Color &color = light->color();
    float intensity = light->intensity();
    //var distance = light.distance;

    Texture::Ptr shadowMap = light->shadow() && light->shadow()->map() ? light->shadow()->map()->texture() : nullptr;

    light::Dispatch dispatch;

    dispatch.func<AmbientLight>() = [&](AmbientLight &alight)
    {
      r += color.r * intensity;
      g += color.g * intensity;
      b += color.b * intensity;
    };
    dispatch.func<DirectionalLight>() = [&](DirectionalLight &directional)
    {
      lights::Entry<DirectionalLight>::Ptr uniforms = cache.get( &directional );

      uniforms->color = light->color() * light->intensity();

      uniforms->direction = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->direction -= math::Vector3::fromMatrixPosition(directional.target()->matrixWorld());
      uniforms->direction.transformDirection( viewMatrix );

      uniforms->shadow = light->castShadow;

      if (light->castShadow) {

        uniforms->shadowBias = light->shadow()->bias();
        uniforms->shadowRadius = light->shadow()->radius();
        uniforms->shadowMapSize = light->shadow()->mapSize();
      }

      state.directionalShadowMap.push_back(shadowMap);
      state.directionalShadowMatrix.push_back(light->shadow()->matrix());
      state.directional.push_back(uniforms);
    };
    dispatch.func<SpotLight>() = [&](SpotLight &spot)
    {
      lights::Entry<SpotLight>::Ptr uniforms = cache.get( &spot );

      uniforms->position = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->position.apply( viewMatrix );

      uniforms->color = color * intensity;
      uniforms->distance = light->distance();

      uniforms->direction = math::Vector3::fromMatrixPosition( light->matrixWorld() );
      uniforms->direction -= math::Vector3::fromMatrixPosition( spot.target()->matrixWorld());
      uniforms->direction.transformDirection( viewMatrix );

      uniforms->coneCos = cos( light->angle() );
      uniforms->penumbraCos = cos( light->angle() * ( 1 - spot.penumbra() ) );
      uniforms->decay = ( light->distance() == 0 ) ? 0.0f : spot.decay();

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
    };
    dispatch.func<RectAreaLight>() = [&](RectAreaLight &area)
    {
      lights::Entry<RectAreaLight>::Ptr uniforms = cache.get( &area );

      // (a) intensity controls irradiance of entire light
      uniforms->color = color  * ( intensity / ( area.width() * area.height()));

      // (b) intensity controls the radiance per light area
      // uniforms.color.copy( color ).multiplyScalar( intensity );

      uniforms->position = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->position.apply( viewMatrix );

      // extract local rotation of light to derive width/height half vectors
      math::Matrix4 matrix4 = light->matrixWorld();
      matrix4.premultiply( viewMatrix );
      math::Matrix4 matrix42 = math::Matrix4 ::rotation( matrix4 );

      uniforms->halfWidth = math::Vector3(area.width() * 0.5, 0.0, 0.0 );
      uniforms->halfHeight = math::Vector3(0.0, area.height() * 0.5, 0.0 );

      uniforms->halfWidth.apply( matrix42 );
      uniforms->halfHeight.apply( matrix42 );

      // TODO (abelnation): RectAreaLight distance?
      // uniforms.distance = distance;

      state.rectArea.push_back(uniforms);
    };
    dispatch.func<PointLight>() = [&](PointLight &point)
    {
      lights::Entry<PointLight>::Ptr uniforms = cache.get( &point );

      uniforms->position = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->position.apply( viewMatrix );

      uniforms->color = light->color() * light->intensity();
      uniforms->distance = light->distance();
      uniforms->decay = (light->distance() == 0) ? 0.0f : point.decay();

      uniforms->shadow = light->castShadow;

      if (light->castShadow) {

        auto shadow = point.shadow();

        uniforms->shadowBias = shadow->bias();
        uniforms->shadowRadius = shadow->radius();
        uniforms->shadowMapSize = shadow->mapSize();
        uniforms->shadowCameraNear = shadow->camera()->near();
        uniforms->shadowCameraFar = shadow->camera()->far();
      }

      state.pointShadowMap.push_back(shadowMap);
      state.pointShadowMatrix.push_back(point.shadow()->matrix());
      state.point.push_back(uniforms);
    };
    dispatch.func<HemisphereLight>() = [&](HemisphereLight &hemisphere)
    {
      lights::Entry<HemisphereLight>::Ptr uniforms = cache.get( &hemisphere );

      uniforms->direction = math::Vector3::fromMatrixPosition( light->matrixWorld() );
      uniforms->direction.transformDirection( viewMatrix );
      uniforms->direction.normalize();

      uniforms->skyColor = light->color() * intensity;
      uniforms->groundColor = hemisphere.groundColor() * intensity;

      state.hemi.push_back(uniforms);
    };

    light->lightResolver->getValue(dispatch);
  }

  state.ambient = Color(r, g, b);

  // TODO (sam-g-steel) why aren't we using join
  stringstream ss;
  ss << state.directional.size() << ',' << state.point.size() << ',' << state.spot.size() << ','
     << state.rectArea.size() << ',' << state.hemi.size();
  state.hash = ss.str();
}

}
}