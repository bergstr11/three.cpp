//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_GLPROGRAM_H
#define THREE_QT_GLPROGRAM_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <Constants.h>
#include <material/Material.h>
#include <material/ShaderMaterial.h>
#include <material/RawShaderMaterial.h>
#include <core/BufferGeometry.h>
#include "shader/ShaderLib.h"
#include "Helpers.h"
#include "Extensions.h"
#include "Uniforms.h"
#include "Attributes.h"

namespace three {

namespace gl {

class ProgramParameters
{
  ProgramParameters()
  {}

public:
  using Ptr = std::shared_ptr<ProgramParameters>;

  static Ptr make()
  { return Ptr(new ProgramParameters()); }

  ShaderID shaderID;
  Precision precision;
  bool supportsVertexTextures = false;
  Encoding outputEncoding = Encoding::Unknown;
  bool map = false;
  Encoding mapEncoding = Encoding::Unknown;
  Texture::Ptr envMap = nullptr;
  Encoding envMapEncoding = Encoding::Unknown;
  TextureMapping envMapMode = TextureMapping::Unknown;
  bool envMapCubeUV = false;
  bool lightMap = false;
  bool aoMap = false;
  bool emissiveMap = false;
  Encoding emissiveMapEncoding = Encoding::Unknown;
  bool bumpMap = false;
  bool normalMap = false;
  bool displacementMap = false;
  bool roughnessMap = false;
  bool metalnessMap = false;
  bool specularMap = false;
  bool alphaMap = false;
  bool gradientMap = false;
  CombineOperation combine = CombineOperation::Unknown;
  Colors vertexColors;
  bool fog = false;
  bool useFog = false;
  bool fogExp = false;
  bool flatShading = false;
  bool logarithmicDepthBuffer = false;
  bool sizeAttenuation = false;
  bool skinning = false;
  size_t maxBones = 0;
  bool useVertexTexture = false;
  bool morphTargets = false;
  bool morphNormals;
  unsigned maxMorphTargets = 0;
  unsigned maxMorphNormals = 0;
  size_t numDirLights = 0;
  size_t numPointLights = 0;
  size_t numSpotLights = 0;
  size_t numRectAreaLights = 0;
  size_t numHemiLights = 0;
  size_t numClippingPlanes = 0;
  size_t numClipIntersection = 0;
  bool dithering = false;
  bool shadowMapEnabled = false;
  ShadowMapType shadowMapType = ShadowMapType::Basic;
  ToneMapping toneMapping = ToneMapping::None;
  UseExtension extensions;
  bool physicallyCorrectLights = false;
  bool premultipliedAlpha = false;
  float alphaTest = 1.0f;
  bool doubleSided = false;
  bool flipSided = false;
  DepthPacking depthPacking = DepthPacking::Unknown;
  std::unordered_map<std::string, std::string> defines;
  RawShaderMaterial *rawShaderMaterial = nullptr;
  ShaderMaterial *shaderMaterial = nullptr;
  std::string index0AttributeName;
};

std::vector<std::string> getEncodingComponents(Encoding encoding);

std::string getTexelDecodingFunction(const char *functionName, Encoding encoding);

std::string getTexelEncodingFunction(const char *functionName, Encoding encoding);

std::string getToneMappingFunction(const char *functionName, ToneMapping toneMapping);

std::string generateDefines(std::unordered_map<std::string, std::string> defines);

std::string generateExtensions(Extensions &extensions, const ProgramParameters &parameters);


class Program
{
public:
  using Ptr = std::shared_ptr<Program>;

private:
  friend class Programs;

  static unsigned programIdCount;

  GLuint _program;

  Renderer_impl &_renderer;

  Uniforms::Ptr _cachedUniforms;

  std::unordered_map<AttributeName, GLint> _cachedAttributes;
  std::unordered_map<IndexedAttributeKey, GLint> _cachedIndexedAttributes;

  void fetchAttributeLocations(std::unordered_map<AttributeName, GLint> &attributes,
                               std::unordered_map<IndexedAttributeKey, GLint> &indexedAttributes);

  Program(Renderer_impl &renderer,
          Extensions &extensions,
          const std::string code,
          const Material::Ptr material,
          Shader &shader,
          const ProgramParameters &parameters);

  static Ptr make(Renderer_impl &renderer,
                  Extensions &extensions,
                  const std::string code,
                  const Material::Ptr material,
                  Shader &shader,
                  const ProgramParameters &parameters)
  {
    return Ptr(new Program(renderer, extensions, code, material, shader, parameters));
  }

public:

  ~Program();

  GLuint id() const
  { return _program; }

  std::string code;

  Uniforms::Ptr getUniforms();

  const std::unordered_map<AttributeName, GLint> &getAttributes();

  const std::unordered_map<IndexedAttributeKey, GLint> &getIndexedAttributes();
};

}
}
#endif //THREE_QT_GLPROGRAM_H
