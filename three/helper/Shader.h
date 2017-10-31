//
// Created by byter on 19.10.17.
//

#ifndef THREE_QT_UNIFORMVALUE_H
#define THREE_QT_UNIFORMVALUE_H

#include <textures/CubeTexture.h>
#include <unordered_map>
#include <initializer_list>

namespace three {

struct UniformValue
{
  enum Type {tCube, glint, flt} type;

  unsigned id;

  union
  {
    float float_val;
    GLint glint_val;
    CubeTexture::Ptr tCube_val;
  };

  UniformValue() {}

  explicit UniformValue(unsigned id, float val) : id(id), type(flt), float_val(val)
  {}

  explicit UniformValue(unsigned id, Type type, GLint val) : id(id), type(glint), glint_val(val)
  {}

  explicit UniformValue(unsigned id, const CubeTexture::Ptr &tex) : id(id), type(tCube), tCube_val(tex)
  {}

  ~UniformValue() {
    switch(type) {
      case tCube:
        tCube_val.reset();
      default:
        break;
    }
  }

  UniformValue &operator =(const UniformValue &other)
  {
    id = other.id;
    if(type == tCube) tCube_val.reset();
    type = other.type;
    switch(type) {
      case tCube:
        tCube_val = other.tCube_val;
        break;
      case flt:
        float_val = other.float_val;
        break;
      case glint:
        glint_val = other.glint_val;
        break;
    }
  }

  UniformValue(const UniformValue &other) : id(other.id), type(other.type)
  {
    switch(type) {
      case tCube:
        tCube_val = other.tCube_val;
        break;
      case flt:
        float_val = other.float_val;
        break;
      case glint:
        glint_val = other.glint_val;
        break;
    }
  }

  void set(const CubeTexture::Ptr &t) {
    if(type != tCube) throw std::invalid_argument("tCube");
    tCube_val = t;
  }
  void set(const GLint &i) {
    if(type != glint) throw std::invalid_argument("glint");
    glint_val = i;
  }
  void set(const float &f) {
    if(type != flt) throw std::invalid_argument("flt");
    float_val = f;
  }
};

template<typename T, unsigned _id>
struct UniformValueBase : public UniformValue
{
  const static unsigned id() {return _id;}

  explicit UniformValueBase(T t) : UniformValue(_id, t)
  {}
};

using tCube = UniformValueBase<CubeTexture::Ptr, 1>;
using tFlip = UniformValueBase<GLint, 2>;
using opacity = UniformValueBase<float, 3>;

class UniformValues
{
  std::unordered_map<unsigned, UniformValue> values;

public:
  UniformValues(std::initializer_list<UniformValue> vals) {
    for(auto it = std::begin(vals); it != std::end(vals); it++) {
      const UniformValue &val = *it;
      values[val.id] = val;
    }
  }

  template <typename T, typename V> void set(const V &v) {
    values[T::id()].set(v);
  }
};

class Shader
{
protected:
  UniformValues _uniforms;
  std::string _name;

public:
  explicit Shader(const std::string &name, const UniformValues &uniforms)
     : _name(name), _uniforms(uniforms) {}

  const UniformValues &uniforms() const {return _uniforms;}

  const std::string name() const {return _name;}

  virtual const std::string vertexShader() = 0;
  virtual const std::string fragmentShader() = 0;
};

}

#endif //THREE_QT_UNIFORMVALUE_H
