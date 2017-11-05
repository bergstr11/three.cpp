//
// Created by byter on 19.10.17.
//

#ifndef THREE_QT_UNIFORMVALUE_H
#define THREE_QT_UNIFORMVALUE_H

#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Vector4.h>
#include <math/Matrix3.h>
#include <math/Matrix4.h>
#include <core/Color.h>
#include <textures/CubeTexture.h>
#include <unordered_map>
#include <initializer_list>

namespace three {

enum class UniformName
{
  cube,
  equirect,
  flip,
  opacity,
  diffuse,
  emissive,
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
  scale
};

struct UniformValue
{
  enum Type {cube, tex, glint, flt, vect2, vect3, vect4, mat3, mat4, color} type;

  UniformName id;

  union
  {
    float float_val;
    GLint glint_val;
    Texture::Ptr tex_val;
    CubeTexture::Ptr cube_val;
    math::Vector2 vect2_val;
    math::Vector3 vect3_val;
    math::Vector4 vect4_val;
    math::Matrix3 mat3_val;
    math::Matrix4 mat4_val;
    Color color_val;
  };

  bool needsUpdate = false;

  UniformValue() {}

  explicit UniformValue(UniformName id, float val) : id(id), type(flt), float_val(val)
  {}
  explicit UniformValue(UniformName id, GLint val) : id(id), type(glint), glint_val(val)
  {}
  explicit UniformValue(UniformName id, const Texture::Ptr &tx) : id(id), type(tex), tex_val(tx)
  {}
  explicit UniformValue(UniformName id, const CubeTexture::Ptr &tx) : id(id), type(cube), cube_val(tx)
  {}
  explicit UniformValue(UniformName id, const math::Vector2 &val) : id(id), type(vect2), vect2_val(val)
  {}
  explicit UniformValue(UniformName id, const math::Vector3 &val) : id(id), type(vect3), vect3_val(val)
  {}
  explicit UniformValue(UniformName id, const math::Vector4 &val) : id(id), type(vect4), vect4_val(val)
  {}
  explicit UniformValue(UniformName id, const math::Matrix3 &val) : id(id), type(mat3), mat3_val(val)
  {}
  explicit UniformValue(UniformName id, const math::Matrix4 &val) : id(id), type(mat4), mat4_val(val)
  {}
  explicit UniformValue(UniformName id, const Color &val) : id(id), type(color), color_val(val)
  {}

  ~UniformValue() {
    switch(type) {
      case tex:
        tex_val.reset();
      case cube:
        cube_val.reset();
      default:
        break;
    }
  }

  UniformValue &operator =(const UniformValue &other)
  {
    id = other.id;
    if(type == tex) tex_val.reset();
    if(type == cube) cube_val.reset();
    type = other.type;
    switch(type) {
      case tex:
        tex_val = other.tex_val;
        break;
      case cube:
        cube_val = other.cube_val;
        break;
      case flt:
        float_val = other.float_val;
        break;
      case glint:
        glint_val = other.glint_val;
        break;
      case vect2:
        vect2_val = other.vect2_val;
        break;
      case vect3:
        vect3_val = other.vect3_val;
        break;
      case vect4:
        vect4_val = other.vect4_val;
        break;
      case mat3:
        mat3_val = other.mat3_val;
        break;
      case mat4:
        mat4_val = other.mat4_val;
        break;
      case color:
        color_val = other.color_val;
        break;
    }
  }

  UniformValue(const UniformValue &other) : id(other.id), type(other.type)
  {
    switch(type) {
      case tex:
        tex_val = other.tex_val;
        break;
      case cube:
        cube_val = other.cube_val;
        break;
      case flt:
        float_val = other.float_val;
        break;
      case glint:
        glint_val = other.glint_val;
        break;
      case vect2:
        vect2_val = other.vect2_val;
        break;
      case vect3:
        vect3_val = other.vect3_val;
        break;
      case vect4:
        vect4_val = other.vect4_val;
        break;
      case mat3:
        mat3_val = other.mat3_val;
        break;
      case mat4:
        mat4_val = other.mat4_val;
        break;
      case color:
        color_val = other.color_val;
        break;
    }
  }

  UniformValue &operator = (const Texture::Ptr &t) {
    if(type != tex) throw std::invalid_argument("invalid type");
    tex_val = t;
  }
  UniformValue &operator = (const CubeTexture::Ptr &t) {
    if(type != cube) throw std::invalid_argument("invalid type");
    cube_val = t;
  }
  UniformValue &operator = (const GLint &i) {
    if(type != glint) throw std::invalid_argument("invalid type");
    glint_val = i;
  }
  UniformValue &operator = (const float &f) {
    if(type != flt) throw std::invalid_argument("invalid type");
    float_val = f;
  }
  UniformValue &operator = (const math::Vector2 &v) {
    if(type != vect2) throw std::invalid_argument("invalid type");
    vect2_val = v;
  }
  UniformValue &operator = (const math::Vector3 &v) {
    if(type != vect3) throw std::invalid_argument("invalid type");
    vect3_val = v;
  }
  UniformValue &operator = (const math::Vector4 &v) {
    if(type != vect4) throw std::invalid_argument("invalid type");
    vect4_val = v;
  }
  UniformValue &operator = (const math::Matrix3 &m) {
    if(type != mat3) throw std::invalid_argument("invalid type");
    mat3_val = m;
  }
  UniformValue &operator = (const math::Matrix4 &m) {
    if(type != mat4) throw std::invalid_argument("invalid type");
    mat4_val = m;
  }
  UniformValue &operator = (const Color &c) {
    if(type != color) throw std::invalid_argument("invalid type");
    color_val = c;
  }
};

class UniformValues
{
  std::unordered_map<UniformName, UniformValue> values;

public:
  UniformValues(std::initializer_list<UniformValue> vals) {
    for(auto it = std::begin(vals); it != std::end(vals); it++) {
      const UniformValue &val = *it;
      values[val.id] = val;
    }
  }

  template <typename T, typename V> void set(const V &v) {
    values[T::id()] = v;
  }

  UniformValue &operator[](UniformName name) {
    return values.at(name);
  }
};

class Shader
{
protected:
  const char * const _name;
  UniformValues _uniforms;

  explicit Shader(const char *name, const UniformValues &uniforms)
     : _name(name), _uniforms(uniforms) {}

  Shader(const Shader &shader) : _name(shader._name), _uniforms(shader._uniforms) {}

public:
  using Ptr = std::shared_ptr<Shader>;

  UniformValues &uniforms() {return _uniforms;}

  virtual const std::string vertexShader() = 0;
  virtual const std::string fragmentShader() = 0;

  const char *name() const {return _name;}
};

}

#endif //THREE_QT_UNIFORMVALUE_H
