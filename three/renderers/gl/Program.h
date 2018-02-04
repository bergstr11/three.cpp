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
#include <three/Constants.h>
#include <three/material/Material.h>
#include <three/material/ShaderMaterial.h>
#include <three/material/RawShaderMaterial.h>
#include <three/core/BufferGeometry.h>
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
  static void hash_combine(size_t &hash, ShaderID value) {
    three::hash_combine(hash, (unsigned)value);
  }
};
template <>
struct Code<Precision> {
  static constexpr Precision init = Precision::unknown;
  static void hash_combine(size_t &hash, Precision value) {
    three::hash_combine(hash, (int)value);
  }
};
template <>
struct Code<bool> {
  static constexpr bool init = false;
  static void hash_combine(size_t &hash, bool value) {
    three::hash_combine(hash, (unsigned)value);
  }
};
template <>
struct Code<Encoding> {
  static constexpr Encoding init = Encoding::Unknown;
  static void hash_combine(size_t &hash, Encoding value) {
    three::hash_combine(hash, (int)value);
  }
};
template <>
struct Code<Texture::Ptr> {
  static const Texture::Ptr init;
  static void hash_combine(size_t &hash, Texture::Ptr value) {
    three::hash_combine(hash, value ? 1 : 0);
  }
};
template <>
struct Code<size_t> {
  static constexpr size_t init = 0;
  static void hash_combine(size_t &hash, bool value) {
    three::hash_combine(hash, value);
  }
};
template <>
struct Code<TextureMapping> {
  static constexpr TextureMapping init = TextureMapping::Unknown;
  static void hash_combine(size_t &hash, TextureMapping value) {
    three::hash_combine(hash, (unsigned)value);
  }
};
template <>
struct Code<CombineOperation> {
  static constexpr CombineOperation init = CombineOperation::Unknown;
  static void hash_combine(size_t &hash, CombineOperation value) {
    three::hash_combine(hash, (unsigned)value);
  }
};
template <>
struct Code<Colors> {
  static constexpr Colors init = Colors::None;
  static void hash_combine(size_t &hash, Colors value) {
    three::hash_combine(hash, (int)value);
  }
};
template <>
struct Code<ShadowMapType> {
  static constexpr ShadowMapType init = ShadowMapType::Basic;
  static void hash_combine(size_t &hash, ShadowMapType value) {
    three::hash_combine(hash, (int)value);
  }
};
template <>
struct Code<ToneMapping> {
  static constexpr ToneMapping init = ToneMapping::None;
  static void hash_combine(size_t &hash, ToneMapping value) {
    three::hash_combine(hash, (int)value);
  }
};
template <>
struct Code<float> {
  static constexpr float init = 0.0;
  static void hash_combine(size_t &hash, float value) {
    three::hash_combine(hash, (unsigned)value);
  }
};
template <>
struct Code<DepthPacking> {
  static constexpr DepthPacking init = DepthPacking::Unknown;
  static void hash_combine(size_t &hash, DepthPacking value) {
    three::hash_combine(hash, (int)value);
  }
};
template <>
struct Code<std::unordered_map<std::string, std::string>> {
  static const std::unordered_map<std::string, std::string> init;
  static void hash_combine(size_t &hash, const std::unordered_map<std::string, std::string> &value) {
    for(const auto &entry : value) {
      three::hash_combine(hash, entry.first);
      three::hash_combine(hash, entry.second);
    }
  }
};


class ProgramParameter
{
public:
  explicit ProgramParameter(std::vector<ProgramParameter *> &all) {
    all.push_back(this);
  }
  virtual void hash_combine(size_t &hash) = 0;

  virtual bool operator == (const ProgramParameter &parameter) = 0;

  bool operator != (const ProgramParameter &parameter) {
    return !(*this == parameter);
  }
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

  operator T &() const {return t;}

  bool operator == (const ProgramParameter &parameter) {
    const ProgramParameterT &pt = dynamic_cast<const ProgramParameterT &>(parameter);
    return t == pt.t;
  }

  void hash_combine(size_t &hash) override {
    Code<T>::hash_combine(hash, t);
  }

  const T &operator *() const {
    return t;
  }
};

enum class ShaderMaterialKind {none, shader, raw};

class ProgramParameters
{
  ProgramParameters()
  {}

public:
  std::vector<ProgramParameter *> all;

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

  ShaderMaterialKind shaderMaterial = ShaderMaterialKind::none;
  bool shaderMaterialClipping = false;
  std::string vertexShader;
  std::string fragmentShader;
  std::string index0AttributeName;
  UseExtension extensions;

  bool operator == (const ProgramParameters &parameters)
  {
    if(vertexShader != parameters.vertexShader) return false;
    if(fragmentShader != parameters.fragmentShader) return false;

    auto it1=all.begin();
    auto it2=parameters.all.begin();
    for(; it1 != all.end() && it2 != parameters.all.end(); it1++, it2++) {
      ProgramParameter *p1 = *it1;
      ProgramParameter *p2 = *it2;
      if(*p1 != *p2) return false;
    }
    return it1 == all.end() && it2 == parameters.all.end();
  }

  bool operator != (const ProgramParameters &parameters)
  {
    return !(*this == parameters);
  }
};

struct parameters_hash {
  std::size_t operator () (const ProgramParameters::Ptr &params) const
  {
    std::size_t h = std::hash<std::string>{}(params->fragmentShader);
    hash_combine(h, params->vertexShader);

    for(auto p : params->all) {
      p->hash_combine(h);
    }
    return h;
  }
};

struct parameters_equal {
  bool operator () (const ProgramParameters::Ptr &left, const ProgramParameters::Ptr &right) const
  {
    return (*left) == (*right);
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
          const Material::Ptr &material,
          Shader &shader,
          ProgramParameters::Ptr parameters);

public:
  static Ptr make(Renderer_impl &renderer,
                  Extensions &extensions,
                  const Material::Ptr material,
                  Shader &shader,
                  const ProgramParameters::Ptr parameters)
  {
    return Ptr(new Program(renderer, extensions, material, shader, parameters));
  }

  ~Program();

  GLuint handle() const
  { return _program; }

  const ProgramParameters::Ptr parameters;

  Uniforms::Ptr getUniforms();

  const std::unordered_map<AttributeName, GLint> &getAttributes();

  const std::unordered_map<IndexedAttributeKey, GLint> &getIndexedAttributes();
};

}
}
#endif //THREE_QT_GLPROGRAM_H
