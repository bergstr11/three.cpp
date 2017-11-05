//
// Created by byter on 11/3/17.
//

#include "UniformsLib.h"
#include "ShaderLib.h"

namespace three {
namespace gl {
namespace uniforms {

using namespace std;

DataTexture::Ptr LTC_MAT_TEXTURE;
DataTexture::Ptr LTC_MAG_TEXTURE;

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

}
}
}
