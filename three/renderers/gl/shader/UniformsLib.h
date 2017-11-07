//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_UNIFORMSLIB_H
#define THREE_QT_UNIFORMSLIB_H

#include <textures/DataTexture.h>
#include <textures/CubeTexture.h>
#include "../Uniforms.h"

namespace three {
namespace gl {

extern DataTexture::Ptr LTC_MAT_TEXTURE;
extern DataTexture::Ptr LTC_MAG_TEXTURE;

/**
 * Uniforms library for RectAreaLight shared webgl shaders
 * @author abelnation
 *
 * NOTE: This is a temporary location for the BRDF approximation texture data
 *       based off of Eric Heitz's work (see citation below).  BRDF data for
 *       RectAreaLight is currently approximated using a precomputed texture
 *       of roughly 80kb in size.  The hope is to find a better way to include
 *       the large texture data before including the full RectAreaLight implementation
 *       in the main build files.
 *
 * TODO: figure out a way to compress the LTC BRDF data
 * TODO: validate the RectAreaLight BRDF and shaders
 */
void initRectAreaLight();

enum class UniformsID {
  common,
  envmap,
  aomap,
  lightmap,
  specularmap,
  emissivemap,
  bumpmap,
  normalmap,
  displacementmap,
  roughnessmap,
  metalnessmap,
  gradientmap,
  fog,
  lights,
  points
};

namespace uniformslib {

template<typename T, UniformName _nm>
struct UniformValueBase : public UniformValue
{
  explicit UniformValueBase(T t) : UniformValue(_nm, t)
  {}
};

using Cube = UniformValueBase<CubeTexture::Ptr, UniformName::cube>;
using Equirect = UniformValueBase<Texture::Ptr, UniformName::equirect>;
using Flip = UniformValueBase<GLint, UniformName::flip>;
using Opacity = UniformValueBase<float, UniformName::opacity>;
using Diffuse = UniformValueBase<Color, UniformName::diffuse>;
using Map = UniformValueBase<Texture::Ptr, UniformName::map>;
using UvTransform = UniformValueBase<math::Matrix3, UniformName::uvTransform>;
using AlphaMap = UniformValueBase<Texture::Ptr, UniformName::alphaMap>;
using SpecularMap = UniformValueBase<Texture::Ptr, UniformName::specularMap>;
using EnvMap = UniformValueBase<Texture::Ptr, UniformName::envMap>;
using FlipEnvMap  = UniformValueBase<GLint, UniformName::flipEnvMap>;
using Reflectivity = UniformValueBase<math::Matrix3, UniformName::reflectivity>;
using RefractionRatio = UniformValueBase<GLfloat, UniformName::refractionRatio>;
using AoMap = UniformValueBase<Texture::Ptr, UniformName::aoMap>;
using AoMapIntensity = UniformValueBase<GLfloat, UniformName::aoMapIntensity>;
using LightMap = UniformValueBase<Texture::Ptr, UniformName::lightMap>;
using LightMapIntensity = UniformValueBase<GLfloat, UniformName::lightMapIntensity>;
using EmissiveMap = UniformValueBase<Texture::Ptr, UniformName::emissiveMap>;
using BumpMap = UniformValueBase<Texture::Ptr, UniformName::bumpMap>;
using BumpScale = UniformValueBase<GLfloat, UniformName::bumpScale>;
using NormalMap = UniformValueBase<Texture::Ptr, UniformName::normalMap>;
using NormalScale = UniformValueBase<math::Vector2, UniformName::normalScale>;
using DisplacementMap = UniformValueBase<Texture::Ptr, UniformName::displacementMap>;
using DisplacementScale = UniformValueBase<GLfloat, UniformName::displacementScale>;
using DisplacementBias = UniformValueBase<GLfloat, UniformName::displacementBias>;
using RoughnessMap = UniformValueBase<Texture::Ptr, UniformName::roughnessMap>;
using MetalnessMap = UniformValueBase<Texture::Ptr, UniformName::metalnessMap>;
using GradientMap = UniformValueBase<Texture::Ptr, UniformName::gradientMap>;
using FogDensity = UniformValueBase<GLfloat, UniformName::fogDensity>;
using FogNear = UniformValueBase<GLfloat, UniformName::fogNear>;
using FogFar = UniformValueBase<GLfloat, UniformName::fogFar>;
using FogColor = UniformValueBase<Color, UniformName::fogColor>;

struct UniformValuesDelegate
{
  UniformValues values;

  explicit UniformValuesDelegate(UniformValues values) : values(values) {}

  operator UniformValues &() const {return *this;}

  UniformValuesDelegate &merge(UniformsID id, std::initializer_list<UniformValue> add);
};

UniformValues &get(UniformsID id);

UniformValuesDelegate merged(std::initializer_list<UniformsID> id);

}

}
}
#endif //THREE_QT_UNIFORMSLIB_H
