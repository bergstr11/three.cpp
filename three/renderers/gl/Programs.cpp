//
// Created by byter on 10/30/17.
//
#include "Renderer_impl.h"
#include "Programs.h"
#include <sstream>

namespace three {
namespace gl {

using namespace std;

ProgramParameters::Ptr Programs::getParameters(const Renderer_impl &renderer,
                                               Material::Ptr material,
                                               Lights::State &lights,
                                               const vector<Light::Ptr> &shadows,
                                               const Fog::Ptr fog,
                                               size_t nClipPlanes,
                                               size_t nClipIntersection,
                                               Object3D::Ptr object)
{
  static const material::ShaderIDs shaderIds;

  ProgramParameters::Ptr parameters = ProgramParameters::make();
  parameters->shaderID = shaderIds.string(*material);

  // heuristics to create shader parameters according to lights in the scene
  // (not to blow over maxLights budget)

  SkinnedMesh::Ptr skinnedMesh = dynamic_pointer_cast<SkinnedMesh>(object);

  size_t maxBones = skinnedMesh ? allocateBones( skinnedMesh ) : 0;
  parameters->precision = _capabilities.precision;

  if (material->precision != Precision::unknown) {

    parameters->precision = _capabilities.getMaxPrecision( material->precision );

    /*if ( precision != material->precision ) {
      console.warn( 'THREE.WebGLProgram.getParameters:', material.precision, 'not supported, using', precision, 'instead.' );
    }*/
  }

  parameters->supportsVertexTextures = _capabilities.vertexTextures;

  const Renderer::Target::Ptr currentRenderTarget = _renderer.getRenderTarget();
  parameters->outputEncoding = currentRenderTarget->texture() ? currentRenderTarget->texture()->encoding() : Encoding::Linear;
  parameters->map = (bool)material->map;
  parameters->mapEncoding = material->map ? material->map->encoding() : Encoding::Linear;
  parameters->envMap = (bool)material->envMap;
  parameters->envMapEncoding = material->envMap ? material->envMap->encoding() : Encoding::Linear;
  parameters->envMapMode = material->envMap ? material->envMap->mapping() : TextureMapping::Unknown;
  parameters->envMapCubeUV = material->envMap &&
                             (material->envMap->mapping() == TextureMapping::CubeUVReflection
                              || material->envMap->mapping() == TextureMapping::CubeUVRefraction);
  parameters->lightMap = (bool)material->lightMap;
  parameters->gradientMap = (bool)material->gradientMap;
  parameters->emissiveMap = (bool)material->emissiveMap;
  parameters->emissiveMapEncoding = material->emissiveMap ? material->emissiveMap->encoding() : Encoding::Linear;
  parameters->displacementMap = (bool)material->displacementMap;
  parameters->vertexColors = material->vertexColors;

  material::Dispatch dispatch;
  dispatch.func<MeshBasicMaterial>() = [parameters] (MeshBasicMaterial &mat) {
    parameters->aoMap = (bool)mat.aoMap;
    parameters->specularMap = (bool)mat.specularMap;
    parameters->combine = mat.combine;
  };
  dispatch.func<MeshDistanceMaterial>() = [parameters] (MeshDistanceMaterial &mat) {
  };
  dispatch.func<MeshDepthMaterial>() = [parameters] (MeshDepthMaterial &mat) {
    parameters->alphaMap = (bool)mat.alphaMap;
    parameters->depthPacking = mat.depthPacking;
  };
  dispatch.func<ShaderMaterial>() = [parameters] (ShaderMaterial &mat) {
    if(mat.use_derivatives)
      parameters->extensions.add(Extension::OES_standard_derivatives);
    if(mat.use_drawBuffers)
      parameters->extensions.add(Extension::GL_EXT_draw_buffers);
    if(mat.use_fragDepth)
      parameters->extensions.add(Extension::EXT_frag_depth);
    if(mat.use_shaderTextureLOD)
      parameters->extensions.add(Extension::EXT_shader_texture_lod);

    parameters->defines.insert(mat.defines.begin(), mat.defines.end());
  };
  dispatch.func<RawShaderMaterial>() = [parameters] (RawShaderMaterial &mat) {
    dispatch.func<ShaderMaterial>()(mat);
    parameters->rawShader = true;
  };
  /*dispatch.func<PointsMaterial>() = [parameters] (MeshDepthMaterial &mat) {
    parameters->sizeAttenuation = (bool)mat.sizeAttenuation;
  };
  dispatch.func<MeshPhongMaterial>() = [&parameters] (MeshPhongMaterial &mat) {
    parameters->aoMap = mat.aoMap;
    parameters->bumpMap = mat.bumpMap;
    parameters->normalMap = mat.normalMap;
    parameters->alphaMap = mat.alphaMap;
  };
  dispatch.func<MeshStandardMaterial>() = [&parameters] (MeshStandardMaterial &mat) {
    parameters->aoMap = mat.aoMap;
    parameters->bumpMap = mat.bumpMap;
    parameters->normalMap = mat.normalMap;
    parameters->roughnessMap = mat.roughnessMap
    parameters->metalnessMap = mat.metalnessMap;
    parameters->alphaMap = mat.alphaMap;
    parameters->defines.insert(mat.defines.begin(), mat.defines.end());
  };
  dispatch.func<MeshNormalMaterial>() = [&parameters] (MeshNormalMaterial &mat) {
    parameters->bumpMap = mat->bumpMap;
    parameters->normalMap = mat.normalMap;
  };
  dispatch.func<MeshLambertMaterial>() = [&parameters] (MeshLambertMaterial &mat) {
    parameters->aoMap = mat.aoMap;
    parameters->alphaMap = mat.alphaMap;
  };*/

  parameters->fog = (bool)fog;
  parameters->useFog = material->fog;
  parameters->fogExp = (bool)dynamic_pointer_cast<FogExp2>(fog);
  parameters->flatShading = material->flatShading;

  parameters->logarithmicDepthBuffer = _capabilities.logarithmicDepthBuffer;

  parameters->skinning = material->skinning() && maxBones > 0;
  parameters->maxBones = maxBones;
  parameters->useVertexTexture = _capabilities.floatVertexTextures;

  parameters->morphTargets = material->morphTargets();
  parameters->morphNormals = material->morphNormals();
  parameters->maxMorphTargets = renderer._maxMorphTargets;
  parameters->maxMorphNormals = renderer._maxMorphNormals;

  parameters->numDirLights = lights.directional.size();
  parameters->numPointLights = lights.point.size();
  parameters->numSpotLights = lights.spot.size();
  parameters->numRectAreaLights = lights.rectArea.size();
  parameters->numHemiLights = lights.hemi.size();

  parameters->numClippingPlanes = nClipPlanes;
  parameters->numClipIntersection = nClipIntersection;

  parameters->dithering = material->dithering;

  parameters->shadowMapEnabled = renderer._shadowMap->enabled() && object->receiveShadow && !shadows.empty();
  parameters->shadowMapType = renderer._shadowMap->type();

  parameters->toneMapping = renderer._toneMapping;
  parameters->physicallyCorrectLights = renderer._physicallyCorrectLights;

  parameters->premultipliedAlpha = material->premultipliedAlpha;

  parameters->alphaTest = material->alphaTest;
  parameters->doubleSided = material->side == Side::Double;
  parameters->flipSided = material->side == Side::Back;

  return parameters;
}

string Programs::getProgramCode(Material::Ptr material, ProgramParameters::Ptr parameters)
{
  static const char *parameterNames[] = {
     "precision", "supportsVertexTextures", "map", "mapEncoding", "envMap", "envMapMode", "envMapEncoding",
     "lightMap", "aoMap", "emissiveMap", "emissiveMapEncoding", "bumpMap", "normalMap", "displacementMap", "specularMap",
     "roughnessMap", "metalnessMap", "gradientMap",
     "alphaMap", "combine", "vertexColors", "fog", "useFog", "fogExp",
     "flatShading", "sizeAttenuation", "logarithmicDepthBuffer", "skinning",
     "maxBones", "useVertexTexture", "morphTargets", "morphNormals",
     "maxMorphTargets", "maxMorphNormals", "premultipliedAlpha",
     "numDirLights", "numPointLights", "numSpotLights", "numHemiLights", "numRectAreaLights",
     "shadowMapEnabled", "shadowMapType", "toneMapping", "physicallyCorrectLights",
     "alphaTest", "doubleSided", "flipSided", "numClippingPlanes", "numClipIntersection", "depthPacking", "dithering"
  };

  ShaderMaterial::Ptr sm = dynamic_pointer_cast<ShaderMaterial>(material);

  stringstream ss;

  if ( parameters->shaderID ) {
    ss << parameters->shaderID << ',';
  }
  else if(sm) {
    ss << sm->fragmentShader << ',' << sm->vertexShader << ',';
  }

  if (sm ) {
    for (auto define : sm->defines ) {
      ss << define.first << ',' << define.second << ',';
    }
  }

  unsigned numNames = sizeof(parameterNames) / sizeof(char *);
  for ( unsigned i = 0; i < numNames; i ++ ) {

    ss << parameters[ parameterNames[ i ] ] );
  }

  array.push( material.onBeforeCompile.toString() );

  array.push( renderer.gammaOutput );

  return ss.str();

}

}
}