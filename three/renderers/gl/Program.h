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
#include <helper/Shader.h>
#include "Helpers.h"
#include "Extensions.h"
#include "Uniforms.h"
#include "Attributes.h"

namespace three {

namespace gl {

class ProgramParameters {
  ProgramParameters() {};

public:
  using Ptr = std::shared_ptr<ProgramParameters>;
  static Ptr make() {return Ptr(new ProgramParameters());}

  std::string shaderID;
  Precision precision;
  bool supportsVertexTextures;
  Encoding outputEncoding;
  bool map;
  Encoding mapEncoding;
  bool envMap;
  Encoding envMapEncoding;
  TextureMapping envMapMode;
  bool envMapCubeUV;
  bool lightMap;
  bool aoMap;
  bool emissiveMap;
  Encoding emissiveMapEncoding;
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
  bool alphaTest;
  bool doubleSided;
  bool flipSided;
  DepthPacking depthPacking;
  std::unordered_map<std::string, std::string> defines;
  bool rawShader;
};

std::vector<std::string> getEncodingComponents(Encoding encoding);

std::string getTexelDecodingFunction(const char *functionName, Encoding encoding);

std::string getTexelEncodingFunction(const char *functionName, Encoding encoding );

std::string getToneMappingFunction(const char *functionName, ToneMapping toneMapping);

std::string generateDefines(std::unordered_map<std::string, std::string> defines);

std::string generateExtensions(Extensions &extensions, const ProgramParameters &parameters);


class Program
{
  friend class Programs;

  static unsigned programIdCount;

  GLuint _program;

  Renderer_impl &_renderer;

  Uniforms::Ptr _cachedUniforms;

  Attributes _cachedAttributes;

  std::unordered_map<std::string, GLint> fetchAttributeLocations();

  Program::Program(Renderer_impl &renderer,
                   Extensions &extensions,
                   const std::string code,
                   const Material::Ptr material,
                   const Shader &shader,
                   const ProgramParameters &parameters );

  using Ptr = std::shared_ptr<Program>;
  static Ptr make(Renderer_impl &renderer,
                  Extensions &extensions,
                  const std::string code,
                  const Material::Ptr material,
                  const Shader &shader,
                  const ProgramParameters &parameters) {
    return Ptr(new Program(renderer, extensions, code, material, shader, parameters));
  }

public:

  ~Program();

  GLuint id() const {return _program;}

  std::string code;

  Uniforms::Ptr getUniforms()
  {
    if (_cachedUniforms == nullptr) {
      _cachedUniforms = Uniforms::make(this, _program);
    }

    return _cachedUniforms;
  }
};

}
}
#endif //THREE_QT_GLPROGRAM_H
