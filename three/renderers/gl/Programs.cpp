//
// Created by byter on 10/30/17.
//
#include "Renderer_impl.h"
#include "Programs.h"

namespace three {
namespace gl {

ProgramParameters::Ptr Programs::getParameters(Material::Ptr material,
                              Lights::State &lights,
                              const std::vector<Light::Ptr> &shadows,
                              const Fog::Ptr fog,
                              size_t nClipPlanes,
                              size_t nClipIntersection,
                              Object3D::Ptr object )
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
  parameters->alphaMap = (bool)material->alphaMap;
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
  };
  /*dispatch.func<MeshPhongMaterial>() = [&parameters] (MeshPhongMaterial &mat) {
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
  /*var parameters = {

    fog: !! fog,
    useFog: material.fog,
    fogExp: ( fog && fog.isFogExp2 ),

    flatShading: material.flatShading,

    sizeAttenuation: material.sizeAttenuation,
    logarithmicDepthBuffer: capabilities.logarithmicDepthBuffer,

    skinning: material.skinning && maxBones > 0,
    maxBones: maxBones,
    useVertexTexture: capabilities.floatVertexTextures,

    morphTargets: material.morphTargets,
    morphNormals: material.morphNormals,
    maxMorphTargets: renderer.maxMorphTargets,
    maxMorphNormals: renderer.maxMorphNormals,

    numDirLights: lights.directional.length,
    numPointLights: lights.point.length,
    numSpotLights: lights.spot.length,
    numRectAreaLights: lights.rectArea.length,
    numHemiLights: lights.hemi.length,

    numClippingPlanes: nClipPlanes,
    numClipIntersection: nClipIntersection,

    dithering: material.dithering,

    shadowMapEnabled: renderer.shadowMap.enabled && object.receiveShadow && shadows.length > 0,
    shadowMapType: renderer.shadowMap.type,

    toneMapping: renderer.toneMapping,
    physicallyCorrectLights: renderer.physicallyCorrectLights,

    premultipliedAlpha: material.premultipliedAlpha,

    alphaTest: material.alphaTest,
    doubleSided: material.side === DoubleSide,
    flipSided: material.side === BackSide,

    depthPacking: ( material.depthPacking !== undefined ) ? material.depthPacking : false

 };*/
  return parameters;
}

}
}