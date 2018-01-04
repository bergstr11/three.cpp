//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_SHADERLIB_H
#define THREE_QT_SHADERLIB_H

#include "ShaderID.h"
#include "UniformsLib.h"

namespace three {
namespace gl {

class UniformValues
{
  std::unordered_map<UniformName, uniformslib::UniformValue::Ptr> values;

public:
  explicit UniformValues(const uniformslib::LibUniformValues &libUniforms) : values(libUniforms.cloneValues()) {}

  bool contains(UniformName name) const {
    return values.find(name) != values.end();
  }

  void needsUpdate(UniformName name, bool value) {
    if(values.count(name) > 0) (*values.at(name)).needsUpdate = value;
  }

  template <typename V>
  UniformValues &set(UniformName name, const V &v) {
    if(values.count(name) > 0)
      *values[name] = v;
    else {
      values[name] = uniformslib::UniformValueT<V>::make(name, v);
    }
  }

  uniformslib::UniformValue &operator[] (UniformName name) {
    return *values.at(name);
  }
};

struct ShaderInfo
{
  const UniformValues uniforms;
  const char * const vertexShader;
  const char * const fragmentShader;

  ShaderInfo(const uniformslib::LibUniformValues &uniforms, const char * vertexShader, const char * fragmentShader);
};

namespace shaderlib {

ShaderInfo get(ShaderID id);

}

}
}
#endif //THREE_QT_SHADERLIB_H
