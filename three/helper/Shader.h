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
  specular,
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
  pointShadowMatrix
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

  UniformValue(UniformName id, float val) : id(id), type(flt), float_val(val)
  {}
  UniformValue(UniformName id, GLint val) : id(id), type(glint), glint_val(val)
  {}
  UniformValue(UniformName id, const Texture::Ptr &tx) : id(id), type(tex), tex_val(tx)
  {}
  UniformValue(UniformName id, const CubeTexture::Ptr &tx) : id(id), type(cube), cube_val(tx)
  {}
  UniformValue(UniformName id, const math::Vector2 &val) : id(id), type(vect2), vect2_val(val)
  {}
  UniformValue(UniformName id, const math::Vector3 &val) : id(id), type(vect3), vect3_val(val)
  {}
  UniformValue(UniformName id, const math::Vector4 &val) : id(id), type(vect4), vect4_val(val)
  {}
  UniformValue(UniformName id, const math::Matrix3 &val) : id(id), type(mat3), mat3_val(val)
  {}
  UniformValue(UniformName id, const math::Matrix4 &val) : id(id), type(mat4), mat4_val(val)
  {}
  UniformValue(UniformName id, const Color &val) : id(id), type(color), color_val(val)
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

  bool contains(UniformName name) const {
    return values.find(name) != values.end();
  }

  template <typename V>
  UniformValues &set(UniformName name, const V &v) {
    values[name] = v;
  }

  UniformValue &operator[](UniformName name) {
    return values.at(name);
  }

  UniformValues &operator +=(const UniformValue &val)
  {
    values.insert({val.id, val});
    return *this;
  }

  UniformValues &operator +=(const UniformValues &vals)
  {
    values.insert(vals.values.begin(), vals.values.end());
    return *this;
  }

  UniformValues merge(const UniformValues &values)
  {
    UniformValues merged(values);
    merged.values.insert(values.values.begin(), values.values.end());
    return merged;
  }
};

class Shader
{
protected:
  const char * _name;
  UniformValues _uniforms;

  std::string _vertexShader;
  std::string _fragmentShader;

public:
  Shader()
     : _name(nullptr), _uniforms({}), _vertexShader(nullptr), _fragmentShader(nullptr) {}

  Shader(const char *name, const UniformValues &uniforms, const std::string vertexShader, const std::string fragmentShader)
     : _name(name), _uniforms(uniforms), _vertexShader(vertexShader), _fragmentShader(fragmentShader) {}

  UniformValues &uniforms() {return _uniforms;}

  const char *vertexShader() {return _vertexShader.data();}
  const char *fragmentShader() {return _fragmentShader.data();}

  const char *name() const {return _name;}
};

}

#endif //THREE_QT_UNIFORMVALUE_H
