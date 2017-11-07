//
// Created by byter on 11/3/17.
//

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

  DataTexture::Options options;
  options.format = TextureFormat::RGBA;
  options.type = TextureType::Float;
  options.mapping = TextureMapping::UV;
  options.wrapS = TextureWrapping::ClampToEdge;
  options.wrapT = TextureWrapping::ClampToEdge;
  options.magFilter = TextureFilter::Linear;
  options.minFilter = TextureFilter::Nearest;
  options.anisotropy = 1;

  LTC_MAT_TEXTURE = DataTexture::make(ltc_mat, 64, 64, options);

  options.format = TextureFormat::Alpha;
  LTC_MAG_TEXTURE = DataTexture::make(ltc_mag, 64, 64, options);

  LTC_MAT_TEXTURE->needsUpdate(true);
  LTC_MAG_TEXTURE->needsUpdate(true);

  // Add ltc tables to materials

  //var ltc_brdf = {ltcMat: {value: null}, ltcMag: {value: null}};

  //Object.assign(ShaderLib.standard.uniforms, ltc_brdf);
  //Object.assign(ShaderLib.physical.uniforms, ltc_brdf);
}

namespace uniformslib {

class UniformsLib
{
  std::unordered_map<UniformsID, UniformValues> _uniforms;

  void add(UniformsID id, const UniformValues &uv)
  {
    _uniforms.insert({id, uv});
  }

public:
  UniformsLib()
  {
    qInitResource();

    add(UniformsID::common,
        UniformValues({
                         Diffuse(Color(0xeeeeee)),
                         Opacity(1.0f),
                         Map(nullptr),
                         UvTransform(math::Matrix3()),
                         AlphaMap(nullptr)
                      }));
    add(UniformsID::specularmap,
        UniformValues({
                         SpecularMap(nullptr)
                      }));
    add(UniformsID::envmap,
        UniformValues({
                         EnvMap(nullptr),
                         FlipEnvMap(-1),
                         Map(nullptr),
                         Reflectivity(math::Matrix3()),
                         RefractionRatio(0.98f)
                      }));
    add(UniformsID::aomap,
        UniformValues({
                         AoMap(nullptr),
                         AoMapIntensity(1)
                      }));
    add(UniformsID::lightmap,
        UniformValues({
                         LightMap(nullptr),
                         LightMapIntensity(1)
                      }));
    add(UniformsID::emissivemap,
        UniformValues({
                         EmissiveMap(nullptr)
                      }));
    add(UniformsID::bumpmap,
        UniformValues({
                         BumpMap(nullptr),
                         BumpScale(1)
                      }));
    add(UniformsID::normalmap,
        UniformValues({
                         NormalMap(nullptr),
                         NormalScale(math::Vector2(1, 1))
                      }));
    add(UniformsID::displacementmap,
        UniformValues({
                         DisplacementMap(nullptr),
                         DisplacementScale(1.0f),
                         DisplacementBias(1)
                      }));
    add(UniformsID::roughnessmap,
        UniformValues({
                         RoughnessMap(nullptr)
                      }));
    add(UniformsID::metalnessmap,
        UniformValues({
                         MetalnessMap(nullptr)
                      }));
    add(UniformsID::gradientmap,
        UniformValues({
                         GradientMap(nullptr)
                      }));
    add(UniformsID::fog,
        UniformValues({
                         FogDensity(0.00025f),
                         FogNear(1),
                         FogFar(2000),
                         FogColor(Color(0xffffff))
                      }));
  }
};

UniformValues &get(UniformsID id)
{
  static UniformValues vals({});

  return vals;
}

UniformValuesDelegate &UniformValuesDelegate::merge(UniformsID id, std::initializer_list<UniformValue> add)
{
  UniformValues extended = get(id);
  for(auto it = std::begin(add); it != std::end(add); it++) {
    extended += *it;
  }
  values += extended;
  return *this;
}

UniformValuesDelegate merged(std::initializer_list<UniformsID> ids)
{
  UniformValues merged {};

  for(auto it = std::begin(ids); it != std::end(ids); it++) {
    merged += get(*it);
  }
  return UniformValuesDelegate(merged);
}

}

}
}
