//
// Created by byter on 10/30/17.
//
#include "Renderer_impl.h"
#include <threepp/material/MeshPhongMaterial.h>
#include <threepp/material/MeshToonMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/material/MeshNormalMaterial.h>
#include <threepp/material/MeshPhysicalMaterial.h>
#include <threepp/material/PointsMaterial.h>

namespace three {
namespace gl {

using namespace std;

Encoding getTextureEncoding(Texture::Ptr map, bool gammaOverrideLinear)
{
  Encoding enc = map ? map->encoding() : Encoding::Linear;

  if (enc == Encoding::Linear && gammaOverrideLinear ) {

    enc = Encoding::Gamma;
  }
  return enc;
}

ProgramParameters::Ptr Programs::getParameters(const Renderer_impl &renderer,
                                               Material::Ptr material,
                                               Lights::State &lights,
                                               const vector<Light::Ptr> &shadows,
                                               const Fog::Ptr fog,
                                               size_t nClipPlanes,
                                               size_t nClipIntersection,
                                               Object3D::Ptr object)
{
  ProgramParameters::Ptr parameters = ProgramParameters::make();
  parameters->shaderID = material->info.shaderId;

  // heuristics to create shader parameters according to lights in the scene
  // (not to blow over maxLights budget)

  SkinnedMesh *skinnedMesh = object->typer;

  size_t maxBones = skinnedMesh ? allocateBones( skinnedMesh ) : 0;
  parameters->precision = _capabilities.precision;

  if (material->precision != Precision::unknown) {
    parameters->precision = _capabilities.getMaxPrecision( material->precision );
  }

  parameters->supportsVertexTextures = _capabilities.vertexTextures;

  const Renderer::Target::Ptr currentRenderTarget = renderer.getRenderTarget();

  parameters->outputEncoding = getTextureEncoding(
     currentRenderTarget ? currentRenderTarget->texture() : nullptr, renderer.gammaOutput);
  parameters->vertexColors = material->vertexColors;

  if(MeshBasicMaterial *mat = material->typer) {
    parameters->map = (bool)mat->map;
    parameters->mapEncoding = getTextureEncoding(mat->map, renderer.gammaInput);

    parameters->aoMap = (bool)mat->aoMap;
    parameters->envMap = (bool)mat->envMap;
    parameters->specularMap = (bool)mat->specularMap;
    parameters->combine = mat->combine;
  }
  else if(MeshDistanceMaterial *mat = material->typer) {
    parameters->map = (bool)mat->map;
    parameters->mapEncoding = getTextureEncoding(mat->map, renderer.gammaInput);

    parameters->alphaMap = (bool)mat->alphaMap;
    parameters->displacementMap = (bool)mat->displacementMap;
  }
  else if(MeshDepthMaterial *mat = material->typer) {
    parameters->map = (bool)mat->map;
    parameters->mapEncoding = getTextureEncoding(mat->map, renderer.gammaInput);

    parameters->alphaMap = (bool)mat->alphaMap;
    parameters->displacementMap = (bool)mat->displacementMap;
    parameters->depthPacking = mat->depthPacking;
  }
  else if(ShaderMaterial *mat = material->typer) {

    if(mat->use_derivatives)
      parameters->extensions.add(Extension::OES_standard_derivatives);
    if(mat->use_drawBuffers)
      parameters->extensions.add(Extension::GLEXT_draw_buffers);
    if(mat->use_fragDepth)
      parameters->extensions.add(Extension::EXT_frag_depth);
    if(mat->use_shaderTextureLOD)
      parameters->extensions.add(Extension::EXT_shader_texture_lod);

    parameters->defines = mat->defines;
    parameters->shaderMaterial = ShaderMaterialKind::shader;
    parameters->fragmentShader = mat->fragmentShader;
    parameters->vertexShader = mat->vertexShader;
    parameters->shaderMaterialClipping = mat->clipping;
    parameters->index0AttributeName = mat->index0AttributeName;

    if(RawShaderMaterial *mat = material->typer) {

      parameters->shaderMaterial = ShaderMaterialKind::raw;
      parameters->fragmentShader = mat->fragmentShader;
      parameters->vertexShader = mat->vertexShader;
    }
  }
  else if(PointsMaterial *mat = material->typer) {
    parameters->map = (bool)mat->map;
    parameters->mapEncoding = getTextureEncoding(mat->map, renderer.gammaInput);

    parameters->sizeAttenuation = (bool)mat->sizeAttenuation;
  }
  else if(MeshPhongMaterial *mat = material->typer) {
    parameters->map = (bool)mat->map;
    parameters->mapEncoding = getTextureEncoding(mat->map, renderer.gammaInput);

    parameters->aoMap = (bool)mat->aoMap;
    parameters->bumpMap = (bool)mat->bumpMap;
    parameters->normalMap = (bool)mat->normalMap;
    parameters->alphaMap = (bool)mat->alphaMap;
    parameters->envMap = (bool)mat->envMap;
    parameters->envMapEncoding = getTextureEncoding(mat->envMap, renderer.gammaInput);
    parameters->envMapMode = mat->envMap ? mat->envMap->mapping() : TextureMapping::Unknown;
    parameters->lightMap = (bool)mat->lightMap;
    parameters->emissiveMap = (bool)mat->emissiveMap;
    parameters->emissiveMapEncoding = getTextureEncoding(mat->emissiveMap, renderer.gammaInput);
    parameters->displacementMap = (bool)mat->displacementMap;

    if(MeshToonMaterial *mat = material->typer) {

      parameters->gradientMap = (bool)mat->gradientMap;
      parameters->defines = mat->defines;
    }
  }
  else if(MeshStandardMaterial *mat = material->typer) {
    parameters->map = (bool)mat->map;
    parameters->mapEncoding = getTextureEncoding(mat->map, renderer.gammaInput);

    parameters->aoMap = (bool)mat->aoMap;
    parameters->bumpMap = (bool)mat->bumpMap;
    parameters->normalMap = (bool)mat->normalMap;
    parameters->roughnessMap = (bool)mat->roughnessMap;
    parameters->metalnessMap = (bool)mat->metalnessMap;
    parameters->alphaMap = (bool)mat->alphaMap;
    parameters->defines = mat->defines;
    parameters->envMap = (bool)mat->envMap;
    parameters->envMapEncoding = mat->envMap ? mat->envMap->encoding() : Encoding::Linear;
    parameters->envMapMode = mat->envMap ? mat->envMap->mapping() : TextureMapping::Unknown;
    parameters->lightMap = (bool)mat->lightMap;
    parameters->displacementMap = (bool)mat->displacementMap;

    if(MeshPhysicalMaterial *mat = material->typer) {

      parameters->defines = mat->defines;
    }
  }
  else if(MeshNormalMaterial *mat = material->typer) {

    parameters->bumpMap = (bool)mat->bumpMap;
    parameters->normalMap = (bool)mat->normalMap;
    parameters->displacementMap = (bool)mat->displacementMap;
  }
  else if(MeshLambertMaterial *mat = material->typer) {
    parameters->map = (bool)mat->map;
    parameters->mapEncoding = getTextureEncoding(mat->map, renderer.gammaInput);

    parameters->aoMap = (bool)mat->aoMap;
    parameters->alphaMap = (bool)mat->alphaMap;
    parameters->envMap = (bool)mat->envMap;
    parameters->envMapEncoding = mat->envMap ? mat->envMap->encoding() : Encoding::Linear;
    parameters->envMapMode = mat->envMap ? mat->envMap->mapping() : TextureMapping::Unknown;
    parameters->specularMap = (bool)mat->specularMap;
    parameters->emissiveMapEncoding = mat->emissiveMap ? mat->emissiveMap->encoding() : Encoding::Linear;
    parameters->combine = mat->combine;
    parameters->lightMap = (bool)mat->lightMap;
  }

  parameters->fog = (bool)fog;
  parameters->useFog = material->fog;
  parameters->fogExp = (bool)dynamic_pointer_cast<FogExp2>(fog);
  parameters->flatShading = material->flatShading;

  parameters->logarithmicDepthBuffer = _capabilities.logarithmicDepthBuffer;

  parameters->skinning = material->skinning && maxBones > 0;
  parameters->maxBones = maxBones;
  parameters->useVertexTexture = _capabilities.floatVertexTextures;

  parameters->morphTargets = material->morphTargets;
  parameters->morphNormals = material->morphNormals;
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

  bool shadowEnabled = renderer._shadowMap.enabled && object->receiveShadow && !shadows.empty();
  parameters->shadowMapEnabled = shadowEnabled;
  parameters->shadowMapType = shadowEnabled ? renderer._shadowMap.type() : ShadowMapType::None;

  parameters->toneMapping = renderer._toneMapping;
  parameters->physicallyCorrectLights = renderer._physicallyCorrectLights;

  parameters->premultipliedAlpha = material->premultipliedAlpha;

  parameters->alphaTest = material->alphaTest;
  parameters->doubleSided = material->side == Side::Double;
  parameters->flipSided = material->side == Side::Back;

  return parameters;
}

}
}
