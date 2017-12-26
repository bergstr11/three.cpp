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


class ProgramParameter
{
public:
  explicit ProgramParameter(std::vector<ProgramParameter *> &all) {
    all.push_back(this);
  }
  virtual void append(std::stringstream &codestream) = 0;
};

template <typename T>
class ProgramParameterT : public ProgramParameter
{
  T t {Code<T>::init};

public:
  explicit ProgramParameterT(std::vector<ProgramParameter *> &all) : ProgramParameter(all) {}

  ProgramParameterT &operator =(const T &t) {
    this->t = t;
  }

  void append(std::stringstream &codestream) override {
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

  std::vector<ProgramParameter *> all;

public:
  using Ptr = std::shared_ptr<ProgramParameters>;

  static Ptr make()
  { return Ptr(new ProgramParameters()); }

  ProgramParameterT<ShaderID>        shaderID {all};
  ProgramParameterT<Precision>       precision {all};
  ProgramParameterT<bool>            supportsVertexTextures {all};
  ProgramParameterT<Encoding>        outputEncoding {all};
  ProgramParameterT<Texture::Ptr>    map {all};
  ProgramParameterT<Encoding>        mapEncoding {all};
  ProgramParameterT<Texture::Ptr>    envMap {all};
  ProgramParameterT<Encoding>        envMapEncoding {all};
  ProgramParameterT<TextureMapping>  envMapMode {all};
  ProgramParameterT<bool>            envMapCubeUV {all};
  ProgramParameterT<Texture::Ptr>    lightMap {all};
  ProgramParameterT<Texture::Ptr>    aoMap {all};
  ProgramParameterT<Texture::Ptr>    emissiveMap {all};
  ProgramParameterT<Encoding>        emissiveMapEncoding {all};
  ProgramParameterT<Texture::Ptr>    bumpMap {all};
  ProgramParameterT<Texture::Ptr>    normalMap {all};
  ProgramParameterT<Texture::Ptr>    displacementMap {all};
  ProgramParameterT<Texture::Ptr>    roughnessMap {all};
  ProgramParameterT<Texture::Ptr>    metalnessMap {all};
  ProgramParameterT<Texture::Ptr>    specularMap {all};
  ProgramParameterT<Texture::Ptr>    alphaMap {all};
  ProgramParameterT<Texture::Ptr>    gradientMap {all};
  ProgramParameterT<CombineOperation> combine {all};
  ProgramParameterT<Colors>          vertexColors {all};
  ProgramParameterT<bool>            fog {all};
  ProgramParameterT<bool>            useFog {all};
  ProgramParameterT<bool>            fogExp {all};
  ProgramParameterT<bool>            flatShading {all};
  ProgramParameterT<bool>            logarithmicDepthBuffer {all};
  ProgramParameterT<bool>            sizeAttenuation {all};
  ProgramParameterT<bool>            skinning {all};
  ProgramParameterT<size_t>          maxBones {all};
  ProgramParameterT<bool>            useVertexTexture {all};
  ProgramParameterT<bool>            morphTargets {all};
  ProgramParameterT<bool>            morphNormals {all};
  ProgramParameterT<size_t>          maxMorphTargets {all};
  ProgramParameterT<size_t>          maxMorphNormals {all};
  ProgramParameterT<size_t>          numDirLights {all};
  ProgramParameterT<size_t>          numPointLights {all};
  ProgramParameterT<size_t>          numSpotLights {all};
  ProgramParameterT<size_t>          numRectAreaLights {all};
  ProgramParameterT<size_t>          numHemiLights {all};
  ProgramParameterT<size_t>          numClippingPlanes {all};
  ProgramParameterT<size_t>          numClipIntersection {all};
  ProgramParameterT<bool>            dithering {all};
  ProgramParameterT<bool>            shadowMapEnabled {all};
  ProgramParameterT<ShadowMapType>   shadowMapType {all};
  ProgramParameterT<ToneMapping>     toneMapping {all};
  ProgramParameterT<bool>            physicallyCorrectLights {all};
  ProgramParameterT<bool>            premultipliedAlpha {all};
  ProgramParameterT<float>           alphaTest {all};
  ProgramParameterT<bool>            doubleSided {all};
  ProgramParameterT<bool>            flipSided {all};
  ProgramParameterT<DepthPacking>  depthPacking {all};
  ProgramParameterT<std::unordered_map<std::string, std::string>> defines {all};

  RawShaderMaterial *rawShaderMaterial = nullptr;
  ShaderMaterial *shaderMaterial = nullptr;
  std::string index0AttributeName;
  UseExtension extensions;

  std::string getProgramCode()
  {
    std::stringstream code;
    if(rawShaderMaterial)
      code << '.' << rawShaderMaterial->vertexShader << rawShaderMaterial->fragmentShader;
    else if(shaderMaterial)
      code << '.' << shaderMaterial->vertexShader << shaderMaterial->fragmentShader;

    for(auto p : all) {
      p->append(code);
    }
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
