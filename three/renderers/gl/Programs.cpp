//
// Created by byter on 10/30/17.
//
#include "Renderer_impl.h"
#include "Programs.h"

namespace three {
namespace gl {

ProgramParameters::Ptr Programs::getParameters(const Renderer_impl &renderer,
                                               Material::Ptr material,
                                               Lights::State &lights,
                                               const std::vector<Light::Ptr> &shadows,
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

  SkinnedMesh::Ptr skinnedMesh = std::dynamic_pointer_cast<SkinnedMesh>(object);

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
  parameters->hasMap = (bool)material->map;
  parameters->mapEncoding = material->map ? material->map->encoding() : Encoding::Linear;
  parameters->hasEnvMap = (bool)material->envMap;
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
  parameters->fogExp = (bool)std::dynamic_pointer_cast<FogExp2>(fog);
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

}
}