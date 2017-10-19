//
// Created by byter on 19.10.17.
//

#ifndef THREE_QT_UNIFORMVALUE_H
#define THREE_QT_UNIFORMVALUE_H

#include <textures/Texture.h>
#include <set>

namespace three {

struct UniformValue
{
  union
  {
    float float_val;
    GLint glint_val;
  } value;

  explicit UniformValue(float val) : value(val)
  {}

  explicit UniformValue(GLint val) : value(val)
  {}
};

template<typename T>
struct UniformValueBase : public UniformValue
{
  explicit UniformValueBase(T t) : UniformValue(t)
  {}
};

using tCube = UniformValueBase<Texture::Ptr>;
using tFlip = UniformValueBase<GLint>;
using opacity = UniformValueBase<float>;

using UniformValues = std::set<UniformValue>;

struct Shader
{
  const UniformValues &uniforms() const;
  const char *vertexShader() const;
  const char *fragmentShader() const;

  Shader(UniformValues uniforms, const char *vertex, const char *fragment);
};

}

#endif //THREE_QT_UNIFORMVALUE_H
