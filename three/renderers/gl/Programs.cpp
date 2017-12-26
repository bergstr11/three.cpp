//
// Created by byter on 10/30/17.
//
#include "Renderer_impl.h"
#include "Programs.h"
#include <sstream>
#include <material/MeshPhongMaterial.h>
#include <material/MeshBasicMaterial.h>
#include <material/MeshDistanceMaterial.h>
#include <material/MeshDepthMaterial.h>
#include <material/MeshToonMaterial.h>
#include <material/MeshLambertMaterial.h>
#include <material/MeshNormalMaterial.h>
#include <material/MeshPhysicalMaterial.h>
#include <material/PointsMaterial.h>

namespace three {
namespace gl {

using namespace std;

Encoding getTextureEncoding(Texture::Ptr map)
{
  return map ? map->encoding() : Encoding::Linear;
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
  static material::ShaderIDs shaderIds;

  ProgramParameters::Ptr parameters = ProgramParameters::make();
  parameters->shaderID = material->resolver->material::ShaderIDsResolver::getValue(shaderIds);

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

  parameters->outputEncoding = getTextureEncoding(currentRenderTarget ? currentRenderTarget->texture() : nullptr);
  parameters->map = material->map;
  parameters->mapEncoding = getTextureEncoding(material->map);
  parameters->vertexColors = material->vertexColors;

  material::Dispatch dispatch;
  dispatch.func<MeshBasicMaterial>() = [parameters] (MeshBasicMaterial &mat) {
    parameters->aoMap = mat.aoMap;
    parameters->envMap = mat.envMap;
    parameters->specularMap = mat.specularMap;
    parameters->combine = mat.combine;
  };
  dispatch.func<MeshDistanceMaterial>() = [parameters] (MeshDistanceMaterial &mat) {
  };
  dispatch.func<MeshDepthMaterial>() = [parameters] (MeshDepthMaterial &mat) {
    parameters->alphaMap = mat.alphaMap;
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

    parameters->defines = mat.defines;
    parameters->shaderMaterial = &mat;
    parameters->index0AttributeName = mat.index0AttributeName;
  };
  dispatch.func<RawShaderMaterial>() = [&] (RawShaderMaterial &mat) {
    dispatch.func<ShaderMaterial>()(mat);
    parameters->rawShaderMaterial = &mat;
  };
  dispatch.func<PointsMaterial>() = [parameters] (PointsMaterial &mat) {
    parameters->sizeAttenuation = (bool)mat.sizeAttenuation;
  };
  dispatch.func<MeshPhongMaterial>() = [&parameters] (MeshPhongMaterial &mat) {
    parameters->aoMap = mat.aoMap;
    parameters->bumpMap = mat.bumpMap;
    parameters->normalMap = mat.normalMap;
    parameters->alphaMap = mat.alphaMap;
    parameters->envMap = mat.envMap;
    parameters->envMapEncoding = mat.envMap ? mat.envMap->encoding() : Encoding::Linear;
    parameters->envMapMode = mat.envMap ? mat.envMap->mapping() : TextureMapping::Unknown;
    parameters->envMapCubeUV = mat.envMap &&
                               (mat.envMap->mapping() == TextureMapping::CubeUVReflection
                                || mat.envMap->mapping() == TextureMapping::CubeUVRefraction);
    parameters->lightMap = mat.lightMap;
    parameters->emissiveMap = mat.emissiveMap;
    parameters->emissiveMapEncoding = mat.emissiveMap ? mat.emissiveMap->encoding() : Encoding::Linear;
    parameters->displacementMap = mat.displacementMap;
  };
  dispatch.func<MeshToonMaterial>() = [&parameters] (MeshToonMaterial &mat) {
    parameters->gradientMap = mat.gradientMap;
    parameters->defines = mat.defines;
  };
  dispatch.func<MeshStandardMaterial>() = [&parameters] (MeshStandardMaterial &mat) {
    parameters->aoMap = mat.aoMap;
    parameters->bumpMap = mat.bumpMap;
    parameters->normalMap = mat.normalMap;
    parameters->roughnessMap = mat.roughnessMap;
    parameters->metalnessMap = mat.metalnessMap;
    parameters->alphaMap = mat.alphaMap;
    parameters->defines = mat.defines;
    parameters->envMap = mat.envMap;
    parameters->envMapEncoding = mat.envMap ? mat.envMap->encoding() : Encoding::Linear;
    parameters->envMapMode = mat.envMap ? mat.envMap->mapping() : TextureMapping::Unknown;
    parameters->envMapCubeUV = mat.envMap &&
                               (mat.envMap->mapping() == TextureMapping::CubeUVReflection
                                || mat.envMap->mapping() == TextureMapping::CubeUVRefraction);
    parameters->lightMap = mat.lightMap;
  };
  dispatch.func<MeshPhysicalMaterial>() = [&] (MeshPhysicalMaterial &mat) {
    dispatch.func<MeshStandardMaterial>()(mat);
    parameters->defines = mat.defines;
  };
  dispatch.func<MeshNormalMaterial>() = [&parameters] (MeshNormalMaterial &mat) {
    parameters->bumpMap = mat.bumpMap;
    parameters->normalMap = mat.normalMap;
  };
  dispatch.func<MeshLambertMaterial>() = [&parameters] (MeshLambertMaterial &mat) {
    parameters->aoMap = mat.aoMap;
    parameters->alphaMap = mat.alphaMap;
    parameters->envMap = mat.envMap;
    parameters->envMapEncoding = mat.envMap ? mat.envMap->encoding() : Encoding::Linear;
    parameters->envMapMode = mat.envMap ? mat.envMap->mapping() : TextureMapping::Unknown;
    parameters->envMapCubeUV = mat.envMap &&
                               (mat.envMap->mapping() == TextureMapping::CubeUVReflection
                                || mat.envMap->mapping() == TextureMapping::CubeUVRefraction);
    parameters->specularMap = mat.specularMap;
    parameters->emissiveMapEncoding = mat.emissiveMap ? mat.emissiveMap->encoding() : Encoding::Linear;
    parameters->combine = mat.combine;
    parameters->lightMap = mat.lightMap;
  };
  material->resolver->material::DispatchResolver::getValue(dispatch);

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

  parameters->shadowMapEnabled = renderer._shadowMap.enabled() && object->receiveShadow && !shadows.empty();
  parameters->shadowMapType = renderer._shadowMap.type();

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
