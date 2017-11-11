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
  {};

public:
  using Ptr = std::shared_ptr<ProgramParameters>;

  static Ptr make()
  { return Ptr(new ProgramParameters()); }

  ShaderID shaderID;
  Precision precision;
  bool supportsVertexTextures;
  Encoding outputEncoding = Encoding::Unknown;
  bool map;
  Encoding mapEncoding = Encoding::Unknown;
  Texture::Ptr envMap;
  Encoding envMapEncoding = Encoding::Unknown;
  TextureMapping envMapMode;
  bool envMapCubeUV;
  bool lightMap;
  bool aoMap;
  bool emissiveMap;
  Encoding emissiveMapEncoding = Encoding::Unknown;
  bool bumpMap;
  bool normalMap;
  bool displacementMap;
  bool roughnessMap;
  bool metalnessMap;
  bool specularMap;
  bool alphaMap;
  bool gradientMap;
  CombineOperation combine;
  Colors vertexColors;
  bool fog;
  bool useFog;
  bool fogExp;
  bool flatShading;
  bool logarithmicDepthBuffer;
  bool sizeAttenuation = false;
  bool skinning;
  size_t maxBones;
  bool useVertexTexture;
  bool morphTargets;
  bool morphNormals;
  unsigned maxMorphTargets;
  unsigned maxMorphNormals;
  size_t numDirLights;
  size_t numPointLights;
  size_t numSpotLights;
  size_t numRectAreaLights;
  size_t numHemiLights;
  size_t numClippingPlanes;
  size_t numClipIntersection;
  bool dithering;
  bool shadowMapEnabled;
  ShadowMapType shadowMapType;
  ToneMapping toneMapping;
  UseExtension extensions;
  bool physicallyCorrectLights;
  bool premultipliedAlpha;
  float alphaTest;
  bool doubleSided;
  bool flipSided;
  DepthPacking depthPacking;
  std::unordered_map<std::string, std::string> defines;
  RawShaderMaterial *rawShaderMaterial = nullptr;
  ShaderMaterial *shaderMaterial = nullptr;
  std::string index0AttributeName = nullptr;
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

  std::unordered_map<std::string, GLint> _cachedAttributes;

  std::unordered_map<std::string, GLint> fetchAttributeLocations();

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

  const std::unordered_map<std::string, GLint> &getAttributes() const;
};

}
}
#endif //THREE_QT_GLPROGRAM_H
