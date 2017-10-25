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

    light::Functions lightFuncs;

    lightFuncs.ambient = [&](AmbientLight &alight)
    {
      r += color.r * intensity;
      g += color.g * intensity;
      b += color.b * intensity;
    };
    lightFuncs.directional = [&](DirectionalLight &dlight)
    {
      uniforms_cache::Entry<DirectionalLight>::Ptr uniforms = cache.get( &dlight );

      uniforms->color = light->color() * light->intensity();

      uniforms->direction = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->direction -= math::Vector3::fromMatrixPosition(dlight.target()->matrixWorld());
      uniforms->direction.transformDirection( viewMatrix );

      uniforms->shadow = light->castShadow();

      if (light->castShadow()) {

        uniforms->shadowBias = light->shadow()->bias();
        uniforms->shadowRadius = light->shadow()->radius();
        uniforms->shadowMapSize = light->shadow()->mapSize();
      }

      state.directionalShadowMap.push_back(shadowMap);
      state.directionalShadowMatrix.push_back(light->shadow()->cmatrix());
      state.directional.push_back(uniforms);
    };
    lightFuncs.spot = [&](SpotLight &slight)
    {
      uniforms_cache::Entry<SpotLight>::Ptr uniforms = cache.get( &slight );

      uniforms->position = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->position.apply( viewMatrix );

      uniforms->color = color * intensity;
      uniforms->distance = light->distance();

      uniforms->direction = math::Vector3::fromMatrixPosition( light->matrixWorld() );
      uniforms->direction -= math::Vector3::fromMatrixPosition( slight.target()->matrixWorld());
      uniforms->direction.transformDirection( viewMatrix );

      uniforms->coneCos = cos( light->angle() );
      uniforms->penumbraCos = cos( light->angle() * ( 1 - slight.penumbra() ) );
      uniforms->decay = ( light->distance() == 0 ) ? 0.0f : slight.decay();

      uniforms->shadow = light->castShadow();

      if (light->castShadow()) {

        LightShadow::Ptr shadow = light->shadow();

        uniforms->shadowBias = shadow->bias();
        uniforms->shadowRadius = shadow->radius();
        uniforms->shadowMapSize = shadow->mapSize();

      }

      state.spotShadowMap.push_back(shadowMap);
      state.spotShadowMatrix.push_back(light->shadow()->cmatrix());
      state.spot.push_back(uniforms);
    };
    lightFuncs.rectarea = [&](RectAreaLight &rlight)
    {
      uniforms_cache::Entry<RectAreaLight>::Ptr uniforms = cache.get( &rlight );

      // (a) intensity controls irradiance of entire light
      uniforms->color = color  * ( intensity / ( rlight.width() * rlight.height()));

      // (b) intensity controls the radiance per light area
      // uniforms.color.copy( color ).multiplyScalar( intensity );

      uniforms->position = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->position.apply( viewMatrix );

      // extract local rotation of light to derive width/height half vectors
      math::Matrix4 matrix4 = light->matrixWorld();
      matrix4.premultiply( viewMatrix );
      math::Matrix4 matrix42 = math::Matrix4 ::rotation( matrix4 );

      uniforms->halfWidth = math::Vector3(rlight.width() * 0.5, 0.0, 0.0 );
      uniforms->halfHeight = math::Vector3(0.0, rlight.height() * 0.5, 0.0 );

      uniforms->halfWidth.apply( matrix42 );
      uniforms->halfHeight.apply( matrix42 );

      // TODO (abelnation): RectAreaLight distance?
      // uniforms.distance = distance;

      state.rectArea.push_back(uniforms);
    };
    lightFuncs.point = [&](PointLight &plight)
    {
      uniforms_cache::Entry<PointLight>::Ptr uniforms = cache.get( &plight );

      uniforms->position = math::Vector3::fromMatrixPosition(light->matrixWorld());
      uniforms->position.apply( viewMatrix );

      uniforms->color = light->color() * light->intensity();
      uniforms->distance = light->distance();
      uniforms->decay = (light->distance() == 0) ? 0.0f : plight.decay();

      uniforms->shadow = light->castShadow();

      if ( light->castShadow() ) {

        auto shadow = plight.shadow();

        uniforms->shadowBias = shadow->bias();
        uniforms->shadowRadius = shadow->radius();
        uniforms->shadowMapSize = shadow->mapSize();
        uniforms->shadowCameraNear = shadow->specificCamera()->near();
        uniforms->shadowCameraFar = shadow->specificCamera()->far();
      }

      state.pointShadowMap.push_back(shadowMap);
      state.pointShadowMatrix.push_back(plight.shadow()->cmatrix());
      state.point.push_back(uniforms);
    };
    lightFuncs.hemisphere = [&](HemisphereLight &hlight)
    {
      uniforms_cache::Entry<HemisphereLight>::Ptr uniforms = cache.get( &hlight );

      uniforms->direction = math::Vector3::fromMatrixPosition( light->matrixWorld() );
      uniforms->direction.transformDirection( viewMatrix );
      uniforms->direction.normalize();

      uniforms->skyColor = light->color() * intensity;
      uniforms->groundColor = hlight.groundColor() * intensity;

      state.hemi.push_back(uniforms);
    };

    light->resolver->call(lightFuncs);
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