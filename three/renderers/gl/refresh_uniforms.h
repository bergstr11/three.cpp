//
// Created by byter on 11/17/17.
//

#ifndef THREE_QT_REFRESH_UNIFORMS_H
#define THREE_QT_REFRESH_UNIFORMS_H

#include <material/Material.h>

namespace three {
namespace gl {

template <typename ... Maps>
struct Refresh;

template <typename Map, typename ... Maps>
struct Refresh<Map, Maps...>
{
  template <typename ... Ms>
  static void mixin(UniformValues &uniforms, MaterialT<Ms...> &material) {
    Refresh<Map>::mixin(uniforms, material);
    Refresh<Maps...>::mixin(uniforms, material);
  }
  static void handle(UniformValues &uniforms, MaterialT<Map, Maps...> &material) {
    Refresh<Map>::mixin(uniforms, material);
    Refresh<Maps...>::mixin(uniforms, material);
  }
};

template <>
struct Refresh<>
{
  static void handle(UniformValues &uniforms, MaterialT<> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, Material &material) {
    uniforms[UniformName::map] = material.map;
  }
};
template <>
struct Refresh<material::Colored>
{
  static void handle(UniformValues &uniforms, MaterialT<material::Colored> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::Colored &material) {
    uniforms[UniformName::diffuse] = material.color;
    uniforms[UniformName::opacity] = material.opacity;
  }
};
template <>
struct Refresh<material::LightMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::LightMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::LightMap &material) {
    uniforms[UniformName::lightMap] = material.lightMap;
    uniforms[UniformName::lightMapIntensity] = material.lightMapIntensity;
  }
};
template <>
struct Refresh<material::EmissiveMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::EmissiveMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::EmissiveMap &material) {
    uniforms[UniformName::emissive] = material.emissive * material.emissiveIntensity;
  }
};
template <>
struct Refresh<material::AoMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::AoMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::AoMap &material) {
    uniforms[UniformName::aoMap] = material.aoMap;
    uniforms[UniformName::aoMapIntensity] = material.aoMapIntensity;
  }
};
template <>
struct Refresh<material::EnvMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::EnvMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::EnvMap &material) {
    uniforms[UniformName::envMap] = material.envMap;

    // don't flip CubeTexture envMaps, flip everything else:
    //  WebGLRenderTargetCube will be flipped for backwards compatibility
    //  WebGLRenderTargetCube.texture will be flipped because it's a Texture and NOT a CubeTexture
    // this check must be handled differently, or removed entirely, if WebGLRenderTargetCube uses a CubeTexture in the future
    //TODO uniforms[UniformName::flipEnvMap] = ( ! ( material.envMap && material.envMap.isCubeTexture ) ) ? 1 : - 1;

    uniforms[UniformName::reflectivity] = material.reflectivity;
    uniforms[UniformName::refractionRatio] = material.refractionRatio;
  }
};
template <>
struct Refresh<material::AlphaMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::AlphaMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::AlphaMap &material) {
    uniforms[UniformName::alphaMap] = material.alphaMap;
  }
};
template <>
struct Refresh<material::SpecularMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::SpecularMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::SpecularMap &material) {
    uniforms[UniformName::specularMap] = material.specularMap;
  }
};
template <>
struct Refresh<material::DisplacementMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::DisplacementMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::DisplacementMap &material) {
    uniforms[UniformName::displacementMap] = material.displacementMap;
  }
};
template <>
struct Refresh<material::BumpMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::BumpMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::BumpMap &material) {
    uniforms[UniformName::bumpMap] = material.bumpMap;
  }
};
template <>
struct Refresh<material::NormalMap>
{
  static void handle(UniformValues &uniforms, MaterialT<material::NormalMap> &material) {
    mixin(uniforms, material);
  }
  static void mixin(UniformValues &uniforms, material::NormalMap &material) {
    uniforms[UniformName::normalMap] = material.normalMap;
  }
};

template <typename ... Maps>
void refresh(UniformValues &uniforms, MaterialT<Maps...> &material)
{
  Refresh<Maps...>::handle(uniforms, material);

  // uv repeat and offset setting priorities
  // 1. color map
  // 2. specular map
  // 3. normal map
  // 4. bump map
  // 5. alpha map
  // 6. emissive map

  Texture::Ptr uvScaleMap;

  if (uniforms.contains(UniformName::map)) {

    uvScaleMap = uniforms[UniformName::map].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::specularMap)) {

    uvScaleMap = uniforms[UniformName::specularMap].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::displacementMap)) {

    uvScaleMap = uniforms[UniformName::displacementMap].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::normalMap)) {

    uvScaleMap = uniforms[UniformName::normalMap].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::bumpMap)) {

    uvScaleMap = uniforms[UniformName::bumpMap].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::roughnessMap)) {

    uvScaleMap = uniforms[UniformName::roughnessMap].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::metalnessMap)) {

    uvScaleMap = uniforms[UniformName::metalnessMap].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::alphaMap)) {

    uvScaleMap = uniforms[UniformName::alphaMap].get<Texture::Ptr>();

  } else if (uniforms.contains(UniformName::emissiveMap)) {

    uvScaleMap = uniforms[UniformName::emissiveMap].get<Texture::Ptr>();

  }

  if ( uvScaleMap ) {

    if ( uvScaleMap->matrixAutoUpdate() ) {

      const math::Vector2 &offset = uvScaleMap->offset();
      const math::Vector2 &repeat = uvScaleMap->repeat();
      float rotation = uvScaleMap->rotation();
      const math::Vector2 &center = uvScaleMap->center();

      uvScaleMap->matrix().setUvTransform( offset.x(), offset.y(), repeat.x(), repeat.y(), rotation, center.x(), center.y());
    }
    uniforms[UniformName::uvTransform] = uvScaleMap->matrix();
  }
}

}
}

#endif //THREE_QT_REFRESH_UNIFORMS_H
