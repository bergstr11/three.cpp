//
// Created by byter on 09.10.17.
//

#ifndef THREE_QT_UNIFORMS_H
#define THREE_QT_UNIFORMS_H

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <array>
#include <memory>
#include <unordered_map>

#include <core/Color.h>
#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Vector4.h>
#include <math/Matrix3.h>
#include <math/Matrix4.h>

#include <textures/ImageTexture.h>
#include <textures/DataTexture.h>
#include <Constants.h>
#include "Helpers.h"
#include "Lights.h"

namespace three {
namespace gl {

enum class UniformName : unsigned
{
  index_0=0,
  index_1=1,
  index_2=2,
  index_3=3,
  index_4=4,
  index_5=5,
  index_6=6,
  index_7=7,
  index_8=8,
  index_9=9,
  index_10=10,
  index_11=11,
  index_12=12,
  index_13=13,
  index_14=14,
  index_15=15,
  tCube,
  tEquirect,
  tFlip,
  opacity,
  diffuse,
  emissive,
  specular,
  shininess,
  projectionMatrix,
  viewMatrix,
  modelViewMatrix,
  normalMatrix,
  modelMatrix,
  logDepthBufFC,
  boneMatrices,
  bindMatrix,
  bindMatrixInverse,
  toneMappingExposure,
  toneMappingWhitePoint,
  cameraPosition,
  map,
  uvTransform,
  alphaMap,
  specularMap,
  envMap,
  flipEnvMap,
  reflectivity,
  refractionRatio,
  aoMap,
  aoMapIntensity,
  lightMap,
  lightMapIntensity,
  emissiveMap,
  bumpMap,
  bumpScale,
  normalMap,
  normalScale,
  displacementMap,
  displacementScale,
  displacementBias,
  roughnessMap,
  metalnessMap,
  gradientMap,
  roughness,
  metalness,
  clearCoat,
  clearCoatRoughness,
  envMapIntensity,
  fogDensity,
  fogNear,
  fogFar,
  fogColor,
  ambientLightColor,
  direction,
  color,
  shadow,
  shadowBias,
  shadowRadius,
  shadowMapSize,
  size,
  scale,
  dashSize,
  totalSize,
  referencePosition,
  nearDistance,
  farDistance,
  clippingPlanes,
  directionalLights,
  spotLights,
  rectAreaLights,
  pointLights,
  hemisphereLights,
  directionalShadowMap,
  directionalShadowMatrix,
  spotShadowMap,
  spotShadowMatrix,
  pointShadowMap,
  pointShadowMatrix,
  distance,
  position,
  coneCos,
  penumbraCos,
  decay
};

UniformName toUniformName(std::string name, bool isIndex=false);

class UniformContainer;
class Renderer_impl;
class UniformValues;

class Uniform
{
  const UniformName _id;
  const GLint _addr;
  const UniformType _type;
  Renderer_impl &_renderer;

protected:
  Uniform(Renderer_impl &renderer, UniformName id, UniformType type, const GLint addr)
     : _id(id), _addr(addr), _type(type), _renderer(renderer) {}

public:
  using Ptr = std::shared_ptr<Uniform>;
  static Ptr make(Renderer_impl &renderer, UniformName id, UniformType type, const GLint addr) {
    return Ptr(new Uniform(renderer, id, type, addr));
  }

  virtual ~Uniform() {}

  const UniformName &id() const {return _id;}

  void setValue(GLfloat v);

  void setValue(GLint v);

  void setValue(GLuint v);

  void setValue(const three::Color &c);

  void setValue(const math::Vector2 &v);

  void setValue(const math::Vector3 &v);

  void setValue(const math::Vector4 &v);

  void setValue(const math::Matrix3 &v);

  void setValue(const math::Matrix4 &v);

  void setValue(const std::vector<float> &vector);

  void setValue(const GLint * array, size_t size);

  void setValue(const Texture::Ptr &texture);

  void setValue(const CubeTexture::Ptr &texture);

  void setValue(const std::vector<math::Matrix4> &matrices);

  void setValue(const std::vector<Texture::Ptr> &textures);

  virtual Uniform *asUniform() {return this;}
  virtual UniformContainer *asContainer() {return nullptr;}
};

class ArrayUniform : public Uniform
{
  const GLint _index;

protected:
  ArrayUniform(Renderer_impl &renderer, UniformName id, UniformType type, const GLint addr)
  : Uniform(renderer, id, type, addr), _index(0) {}

public:
  using Ptr = std::shared_ptr<ArrayUniform>;
  static Ptr make(Renderer_impl &renderer, UniformName id, UniformType type, const GLint addr) {
    return Ptr(new ArrayUniform(renderer, id, type, addr));
  }
};

class UniformContainer
{
  friend class Uniforms;

protected:
  std::vector<UniformName> _sequence;
  std::unordered_map<UniformName, Uniform::Ptr> _map;

public:
  virtual ~UniformContainer() {}

  void add(Uniform::Ptr uniform) {
    _sequence.push_back(uniform->id());
    _map[uniform->id()] = uniform;
  }

  const std::vector<UniformName> &sequence() {return _sequence;}

  template <typename T>
  void set(UniformName name, const T &value) {
    if(_map.count(name)) _map.at(name)->setValue(value);
  }

  Uniform::Ptr get(UniformName name) {
    return _map.count(name) ? _map.at(name) : nullptr;
  }

  Uniform::Ptr get(size_t index)
  {
    switch(index) {
      case 0: return get(UniformName::index_0);
      case 1: return get(UniformName::index_1);
      case 2: return get(UniformName::index_2);
      case 3: return get(UniformName::index_3);
      case 4: return get(UniformName::index_4);
      case 5: return get(UniformName::index_5);
      case 6: return get(UniformName::index_6);
      case 7: return get(UniformName::index_7);
      case 8: return get(UniformName::index_8);
      case 9: return get(UniformName::index_9);
      case 10: return get(UniformName::index_10);
      case 11: return get(UniformName::index_11);
      case 12: return get(UniformName::index_12);
      case 13: return get(UniformName::index_13);
      case 14: return get(UniformName::index_14);
      case 15: return get(UniformName::index_15);
      default:
        throw std::logic_error("unsupported index");
    }
  }
};

class StructuredUniform : public Uniform, public UniformContainer
{
protected:
  StructuredUniform(Renderer_impl &renderer, UniformName id, UniformType type, const GLint addr)
  : Uniform(renderer, id, type, addr) {}

public:
  using Ptr = std::shared_ptr<StructuredUniform>;
  static Ptr make(Renderer_impl &renderer, UniformName id, UniformType type, const GLint addr) {
    return Ptr(new StructuredUniform(renderer, id, type, addr));
  }

  Uniform *asUniform() override {return nullptr;}
  UniformContainer *asContainer() override {return this;}
};

/**
 * @author tschw
 *
 * Uniforms of a program.
 * Those form a tree structure with a special top-level container for the root,
 * which you get by calling 'new WebGLUniforms(program, renderer )'.
 *
 *
 * Properties of inner nodes including the top-level container:
 *
 * .seq - array of nested uniforms
 * .map - nested uniforms by name
 *
 *
 * Methods of all nodes except the top-level container:
 *
 * .setValue(value, [renderer] )
 *
 * 		uploads a uniform value(s)
 *  	the 'renderer' parameter is needed for sampler uniforms
 *
 *
 * Static methods of the top-level container (renderer factorizations):
 *
 * .upload(seq, values, renderer )
 *
 * 		sets uniforms in 'seq' to 'values[id].value'
 *
 * .seqWithValue( seq, values ) : filteredSeq
 *
 * 		filters 'seq' entries with corresponding entry in values
 *
 *
 * Methods of the top-level container (renderer factorizations):
 *
 * .setValue(name, value )
 *
 * 		sets uniform with  name 'name' to 'value'
 *
 * .set(obj, prop )
 *
 * 		sets uniform from object and property with same name than uniform
 *
 * .setOptional(obj, prop )
 *
 * 		like .set for an optional property of the object
 *
 */
class Uniforms : public UniformContainer
{
  // --- Utilities ---
  Renderer_impl & _renderer;

  // Array Caches (provide typed arrays for temporary by size)
  std::unordered_map<size_t, std::vector<float>> arrayCacheF32;
  std::unordered_map<size_t, std::vector<int32_t>> arrayCacheI32;

  // Float32Array caches used for uploading Matrix uniforms
  std::array<float, 16> mat4array;
  std::array<float, 9> mat3array;

  // Flattening for arrays of vectors and matrices
  template <typename T>
  std::vector<float> &flatten(const std::vector<T> array)
  {
    size_t n = sizeof(T) * array.size();

    if(arrayCacheF32.find(n) == arrayCacheF32.end()) {
      arrayCacheF32.emplace(n, std::vector<float>(n));
      arrayCacheF32[n].resize(n);
    }
    auto &r = arrayCacheF32[ n ];

    size_t offset = 0;
    for (const T &t : array) {

      t.writeTo(r.data(), offset);
      offset += sizeof(T);
    }

    return r;
  }

  // Texture unit allocation
  std::vector<int32_t> &allocTexUnits(Renderer_impl &renderer, size_t n);

  void parseUniform(GLuint program, unsigned index, UniformContainer *container);

  Uniforms(Renderer_impl & renderer, GLuint program);

  const std::string _id = "uniforms";

public:
  using Ptr = std::shared_ptr<Uniforms>;
  static Ptr make(Renderer_impl & renderer, GLuint program) {
    return Ptr(new Uniforms(renderer, program));
  }

  template <typename T>
  void set(UniformName name, T value) {
    if(_map.count(name) > 0) {
      _map[name]->setValue(value);
    }
  }

  const Uniform *get(UniformName name) const
  {
    if(_map.count(name) > 0) {
      return _map.at(name)->asUniform();
    }
    return nullptr;
  }

  //seqWithValue
  std::vector<Uniform::Ptr> sequenceUniforms(const UniformValues &values);
};

}
}
#endif //THREE_QT_UNIFORMS_H
