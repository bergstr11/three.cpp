//
// Created by byter on 11/3/17.
//

#include <threepp/util/osdecl.h>
#include "UniformsLib.h"
#include "ShaderLib.h"

static void qInitResource()
{
  static bool done = false;

  if (done) return;
  done = true;

  //must do this outside of namespace - hence this function
  Q_INIT_RESOURCE(Materials);
}

namespace three {
namespace gl {

DataTexture::Ptr LTC_MAT_TEXTURE;
DataTexture::Ptr LTC_MAG_TEXTURE;

using namespace std;

vector<float> loadTexture(const char *name)
{
  vector<float> result(100);
  return result;
}

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

// Precomputed values for linearly transformed cosines
// Taken from "Real-Time Polygonal-Light Shading with Linearly Transformed Cosines"
// By Eric Heitz, Jonathan Dupuy, Stephen Hill and David Neubelt
// https://eheitzresearch.wordpress.com/415-2/

void initRectAreaLight()
{
  //var UniformsLib = THREE.UniformsLib;
  //var ShaderLib = THREE.ShaderLib;

  vector<float> ltc_mat = loadTexture("LTC_MAT");
  vector<float> ltc_mag = loadTexture("LTC_MAG");

  TextureOptions options = DataTexture::options();
  options.format = TextureFormat::RGBA;
  options.type = TextureType::Float;
  options.mapping = TextureMapping::UV;
  options.wrapS = TextureWrapping::ClampToEdge;
  options.wrapT = TextureWrapping::ClampToEdge;
  options.magFilter = TextureFilter::Linear;
  options.minFilter = TextureFilter::Nearest;
  options.anisotropy = 1;

  LTC_MAT_TEXTURE = DataTexture::make(options, ltc_mat, 64, 64);

  options.format = TextureFormat::Alpha;
  LTC_MAG_TEXTURE = DataTexture::make(options, ltc_mag, 64, 64);

  LTC_MAT_TEXTURE->needsUpdate();
  LTC_MAG_TEXTURE->needsUpdate();

  // Add ltc tables to materials

  //var ltc_brdf = {ltcMat: {value: null}, ltcMag: {value: null}};

  //Object.assign(ShaderLib.standard.uniforms, ltc_brdf);
  //Object.assign(ShaderLib.physical.uniforms, ltc_brdf);
}

namespace uniformslib {

class UniformsLib
{
  enum_map<UniformsID, LibUniformValues> _uniforms;

  void add(UniformsID id, const LibUniformValues &uv)
  {
    _uniforms.insert({id, uv});
  }

public:
  const LibUniformValues &uniforms(UniformsID id) const {return _uniforms.at(id);}

  UniformsLib()
  {
    qInitResource();

    add(UniformsID::common,
        LibUniformValues({
                         value<Color>(UniformName::diffuse, Color(0xeeeeee)),
                         value<float>(UniformName::opacity, 1.0f),
                         value<Texture::Ptr>(UniformName::map, nullptr),
                         value<math::Matrix3>(UniformName::uvTransform, math::Matrix3()),
                         value<Texture::Ptr>(UniformName::alphaMap, nullptr)
                         }));
    add(UniformsID::specularmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::specularMap, nullptr)
                         }));
    add(UniformsID::envmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::envMap, nullptr),
                         value<float>(UniformName::envMapIntensity, 1.0f),
                         value<float>(UniformName::flipEnvMap, -1.0f),
                         value<Texture::Ptr>(UniformName::map, nullptr),
                         value<float>(UniformName::reflectivity, 1.0f),
                         value<GLfloat>(UniformName::refractionRatio, 0.98f)
                         }));
    add(UniformsID::aomap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::aoMap, nullptr),
                         value<GLfloat>(UniformName::aoMapIntensity, 1)
                         }));
    add(UniformsID::lightmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::lightMap, nullptr),
                         value<GLfloat>(UniformName::lightMapIntensity, 1)
                         }));
    add(UniformsID::emissivemap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::emissiveMap, nullptr)
                         }));
    add(UniformsID::bumpmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::bumpMap, nullptr),
                         value<GLfloat>(UniformName::bumpScale, 1)
                         }));
    add(UniformsID::normalmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::normalMap, nullptr),
                         value<math::Vector2>(UniformName::normalScale, math::Vector2(1, 1))
                         }));
    add(UniformsID::displacementmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::displacementMap, nullptr),
                         value<GLfloat>(UniformName::displacementScale, 1.0f),
                         value<GLfloat>(UniformName::displacementBias, 1)
                         }));
    add(UniformsID::roughnessmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::roughnessMap, nullptr)
                         }));
    add(UniformsID::metalnessmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::metalnessMap, nullptr)
                         }));
    add(UniformsID::gradientmap,
        LibUniformValues({
                         value<Texture::Ptr>(UniformName::gradientMap, nullptr)
                         }));
    add(UniformsID::fog,
        LibUniformValues({
                         value<GLfloat>(UniformName::fogDensity, 0.00025f),
                         value<GLfloat>(UniformName::fogNear, 1),
                         value<GLfloat>(UniformName::fogFar, 2000),
                         value<Color>(UniformName::fogColor, Color(0xffffff))
                         }));
    add(UniformsID::lights,
        LibUniformValues({
                         value<Color>(UniformName::ambientLightColor, Color(ColorName::white)),
                         value<CachedDirectionalLights>(UniformName::directionalLights, CachedDirectionalLights(), {
                            value<math::Vector3>(UniformName::direction, math::Vector3()),
                            value<Color>(UniformName::color, Color::null()),
                            value<bool>(UniformName::shadow, false),
                            value<float>(UniformName::shadowBias, 0.0f),
                            value<float>(UniformName::shadowRadius, 0.0f),
                            value<math::Vector2>(UniformName::shadowMapSize, math::Vector2()),
                         }),

                         value<std::vector<Texture::Ptr>>(UniformName::directionalShadowMap, std::vector<Texture::Ptr>()),
                         value<std::vector<math::Matrix4>>(UniformName::directionalShadowMatrix, std::vector<math::Matrix4>()),

                         value<CachedSpotLights>(UniformName::spotLights, CachedSpotLights(), {
                            value<Color>(UniformName::color, Color::null()),
                            value<math::Vector3>(UniformName::position, math::Vector3()),
                            value<math::Vector3>(UniformName::direction, math::Vector3()),
                            value<math::Vector3>(UniformName::distance, math::Vector3()),
                            value<float>(UniformName::coneCos, 0.0f),
                            value<float>(UniformName::penumbraCos, 0.0f),
                            value<float>(UniformName::decay, 0.0f),
                            value<bool>(UniformName::shadow, false),
                            value<float>(UniformName::shadowBias, 0.0f),
                            value<float>(UniformName::shadowRadius, 0.0f),
                            value<math::Vector2>(UniformName::shadowMapSize, math::Vector2()),
                         }),

                         value<std::vector<Texture::Ptr>>(UniformName::spotShadowMap, std::vector<Texture::Ptr>()),
                         value<std::vector<math::Matrix4>>(UniformName::spotShadowMatrix, std::vector<math::Matrix4>()),

                         value<CachedPointLights>(UniformName::pointLights, CachedPointLights(), {
                            value<Color>(UniformName::color, Color::null()),
                            value<math::Vector3>(UniformName::position, math::Vector3()),
                            value<float>(UniformName::distance, 0.0f),
                            value<float>(UniformName::decay, 0.0f),
                            value<bool>(UniformName::shadow, false),
                            value<float>(UniformName::shadowBias, 0.0f),
                            value<float>(UniformName::shadowRadius, 0.0f),
                            value<math::Vector2>(UniformName::shadowMapSize, math::Vector2()),
                            value<float>(UniformName::shadowCameraNear, 1.0f),
                            value<float>(UniformName::shadowCameraFar, 1000),
                         }),

                         value<std::vector<Texture::Ptr>>(UniformName::pointShadowMap, std::vector<Texture::Ptr>()),
                         value<std::vector<math::Matrix4>>(UniformName::pointShadowMatrix, std::vector<math::Matrix4>()),

                         value<CachedHemisphereLights>(UniformName::hemisphereLights, CachedHemisphereLights(), {
                            value<math::Vector3>(UniformName::direction, math::Vector3()),
                            value<Color>(UniformName::skyColor, Color::null()),
                            value<Color>(UniformName::groundColor, Color::null()),
                         }),
                         }));
    add(UniformsID::points,
        LibUniformValues({
                            value<Color>(UniformName::diffuse, Color( 0xeeeeee )),
                            value<GLfloat>(UniformName::opacity, 1.0f),
                            value<GLfloat>(UniformName::size, 1.0f),
                            value<GLfloat>(UniformName::scale, 1.0f),
                            value<Texture::Ptr>(UniformName::map, nullptr),
                            value<math::Matrix3>(UniformName::uvTransform, math::Matrix3())
                         }));
  }
};

const LibUniformValues &get(UniformsID id)
{
  static UniformsLib uniformsLib;

  return uniformsLib.uniforms(id);
}

UniformValuesDelegate &UniformValuesDelegate::add(std::initializer_list<UniformValueDelegate> add)
{
  for(auto it = std::begin(add); it != std::end(add); it++) {
    values += (UniformValue::Ptr)*it;
  }
  return *this;
}

UniformValuesDelegate merged(std::initializer_list<UniformsID> ids)
{
  UniformValuesDelegate merged;

  for(auto it = std::begin(ids); it != std::end(ids); it++) {
    merged.values += get(*it);
  }
  return merged;
}

}

}
}
