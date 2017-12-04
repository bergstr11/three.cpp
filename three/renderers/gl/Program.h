//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_GLPROGRAM_H
#define THREE_QT_GLPROGRAM_H

#include <memory>
#include <string>
#include <sstream>
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

template <typename T>
struct Code;

template <>
struct Code<ShaderID> {
  static constexpr ShaderID init = ShaderID::undefined;
  static void append(ShaderID value, std::stringstream &sstream) {
    sstream << (int)value;
  }
};
template <>
struct Code<Precision> {
  static constexpr Precision init = Precision::unknown;
  static void append(Precision value, std::stringstream &sstream) {
    sstream << (int)value;
  }
};
template <>
struct Code<bool> {
  static constexpr bool init = false;
  static void append(bool value, std::stringstream &sstream) {
    sstream << (int)value;
  }
};
template <>
struct Code<Encoding> {
  static constexpr Encoding init = Encoding::Unknown;
  static void append(Encoding value, std::stringstream &sstream) {
    sstream << (int)value;
  }
};
template <>
struct Code<Texture::Ptr> {
  static const Texture::Ptr init;
  static void append(Texture::Ptr value, std::stringstream &sstream) {
    sstream << value ? 1 : 0;
  }
};
template <>
struct Code<size_t> {
  static constexpr size_t init = 0;
  static void append(size_t value, std::stringstream &sstream) {
    sstream << value;
  }
};
template <>
struct Code<TextureMapping> {
  static constexpr TextureMapping init = TextureMapping::Unknown;
  static void append(TextureMapping value, std::stringstream &sstream) {
    sstream << (GLenum)value;
  }
};
template <>
struct Code<CombineOperation> {
  static constexpr CombineOperation init = CombineOperation::Unknown;
  static void append(CombineOperation value, std::stringstream &sstream) {
    sstream << (GLenum)value;
  }
};
template <>
struct Code<Colors> {
  static constexpr Colors init = Colors::None;
  static void append(Colors value, std::stringstream &sstream) {
    sstream << (GLenum)value;
  }
};
template <>
struct Code<ShadowMapType> {
  static constexpr ShadowMapType init = ShadowMapType::Basic;
  static void append(ShadowMapType value, std::stringstream &sstream) {
    sstream << (GLenum)value;
  }
};
template <>
struct Code<ToneMapping> {
  static constexpr ToneMapping init = ToneMapping::None;
  static void append(ToneMapping value, std::stringstream &sstream) {
    sstream << (GLenum)value;
  }
};
template <>
struct Code<float> {
  static constexpr float init = 0.0;
  static void append(float value, std::stringstream &sstream) {
    sstream << value;
  }
};
template <>
struct Code<DepthPacking> {
  static constexpr DepthPacking init = DepthPacking::Unknown;
  static void append(DepthPacking value, std::stringstream &sstream) {
    sstream << (GLenum)value;
  }
};
template <>
struct Code<std::unordered_map<std::string, std::string>> {
  static const std::unordered_map<std::string, std::string> init;
  static void append(const std::unordered_map<std::string, std::string> &value, std::stringstream &sstream) {
    for(auto &val : value) {
      sstream << val.first << '=' << val.second;
    }
  }
};



template <typename T>
class ProgramParameter
{
  T t {Code<T>::init};
  std::stringstream &codestream;
public:
  explicit ProgramParameter(std::stringstream &codestream) : codestream(codestream) {}

  ProgramParameter &operator =(const T &t) {
    this->t = t;
    if(codestream.tellp() > 0) codestream << '.';
    Code<T>::append(t, codestream);
  }

  const T &operator *() const {
    return t;
  }
};

class ProgramParameters
{
  ProgramParameters()
  {}

  std::stringstream code;
public:
  using Ptr = std::shared_ptr<ProgramParameters>;

  static Ptr make()
  { return Ptr(new ProgramParameters()); }

  ProgramParameter<ShaderID>        shaderID {code};
  ProgramParameter<Precision>       precision {code};
  ProgramParameter<bool>            supportsVertexTextures {code};
  ProgramParameter<Encoding>        outputEncoding {code};
  ProgramParameter<Texture::Ptr>    map {code};
  ProgramParameter<Encoding>        mapEncoding {code};
  ProgramParameter<Texture::Ptr>    envMap {code};
  ProgramParameter<Encoding>        envMapEncoding {code};
  ProgramParameter<TextureMapping>  envMapMode {code};
  ProgramParameter<bool>            envMapCubeUV {code};
  ProgramParameter<Texture::Ptr>    lightMap {code};
  ProgramParameter<Texture::Ptr>    aoMap {code};
  ProgramParameter<Texture::Ptr>    emissiveMap {code};
  ProgramParameter<Encoding>        emissiveMapEncoding {code};
  ProgramParameter<Texture::Ptr>    bumpMap {code};
  ProgramParameter<Texture::Ptr>    normalMap {code};
  ProgramParameter<Texture::Ptr>    displacementMap {code};
  ProgramParameter<Texture::Ptr>    roughnessMap {code};
  ProgramParameter<Texture::Ptr>    metalnessMap {code};
  ProgramParameter<Texture::Ptr>    specularMap {code};
  ProgramParameter<Texture::Ptr>    alphaMap {code};
  ProgramParameter<Texture::Ptr>    gradientMap {code};
  ProgramParameter<CombineOperation> combine {code};
  ProgramParameter<Colors>          vertexColors {code};
  ProgramParameter<bool>            fog {code};
  ProgramParameter<bool>            useFog {code};
  ProgramParameter<bool>            fogExp {code};
  ProgramParameter<bool>            flatShading {code};
  ProgramParameter<bool>            logarithmicDepthBuffer {code};
  ProgramParameter<bool>            sizeAttenuation {code};
  ProgramParameter<bool>            skinning {code};
  ProgramParameter<size_t>          maxBones {code};
  ProgramParameter<bool>            useVertexTexture {code};
  ProgramParameter<bool>            morphTargets {code};
  ProgramParameter<bool>            morphNormals {code};
  ProgramParameter<size_t>          maxMorphTargets {code};
  ProgramParameter<size_t>          maxMorphNormals {code};
  ProgramParameter<size_t>          numDirLights {code};
  ProgramParameter<size_t>          numPointLights {code};
  ProgramParameter<size_t>          numSpotLights {code};
  ProgramParameter<size_t>          numRectAreaLights {code};
  ProgramParameter<size_t>          numHemiLights {code};
  ProgramParameter<size_t>          numClippingPlanes {code};
  ProgramParameter<size_t>          numClipIntersection {code};
  ProgramParameter<bool>            dithering {code};
  ProgramParameter<bool>            shadowMapEnabled {code};
  ProgramParameter<ShadowMapType>   shadowMapType {code};
  ProgramParameter<ToneMapping>     toneMapping {code};
  ProgramParameter<bool>            physicallyCorrectLights {code};
  ProgramParameter<bool>            premultipliedAlpha {code};
  ProgramParameter<float>           alphaTest {code};
  ProgramParameter<bool>            doubleSided {code};
  ProgramParameter<bool>            flipSided {code};
  ProgramParameter<DepthPacking>  depthPacking {code};
  ProgramParameter<std::unordered_map<std::string, std::string>> defines {code};

  RawShaderMaterial *rawShaderMaterial = nullptr;
  ShaderMaterial *shaderMaterial = nullptr;
  std::string index0AttributeName;
  UseExtension extensions;

  std::string getProgramCode() {
    if(rawShaderMaterial)
      code << '.' << rawShaderMaterial->vertexShader << rawShaderMaterial->fragmentShader;
    if(shaderMaterial)
      code << '.' << shaderMaterial->vertexShader << shaderMaterial->fragmentShader;

    return code.str();
  }
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
          const std::string &code,
          const Material::Ptr &material,
          Shader &shader,
          const ProgramParameters &parameters);

public:
  static Ptr make(Renderer_impl &renderer,
                  Extensions &extensions,
                  const std::string code,
                  const Material::Ptr material,
                  Shader &shader,
                  const ProgramParameters &parameters)
  {
    return Ptr(new Program(renderer, extensions, code, material, shader, parameters));
  }

  ~Program();

  GLuint handle() const
  { return _program; }

  const std::string code;

  Uniforms::Ptr getUniforms();

  const std::unordered_map<AttributeName, GLint> &getAttributes();

  const std::unordered_map<IndexedAttributeKey, GLint> &getIndexedAttributes();
};

}
}
#endif //THREE_QT_GLPROGRAM_H
