//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_SHADERLIB_H
#define THREE_QT_SHADERLIB_H

#include <QString>
#include <QResource>

#include "UniformsLib.h"

namespace three {
namespace gl {

enum class ShaderID : unsigned
{
  basic=0,
  lambert=1,
  phong=2,
  standard=3,
  normal=4,
  points=5,
  dashed=6,
  depth=7,
  cube=8,
  equirect=9,
  distanceRGBA=10,
  shadow=11,
  physical=12,

  undefined=999
};

class UniformValues
{
  std::unordered_map<UniformName, uniformslib::UniformValue::Ptr> values;

public:
  explicit UniformValues(const uniformslib::LibUniformValues &libUniforms) : values(libUniforms.cloneValues()) {}

  bool contains(UniformName name) const {
    return values.find(name) != values.end();
  }

  template <typename V>
  UniformValues &set(UniformName name, const V &v) {
    *values[name] = v;
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
