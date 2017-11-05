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

#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Vector4.h>
#include <math/Matrix3.h>
#include <math/Matrix4.h>

#include <Constants.h>
#include <helper/Shader.h>

namespace three {
namespace gl {

class UniformContainer;
class Renderer_impl;

class Uniform
{
  const UniformName _id;
  const GLint _addr;
  const UniformType _type;
  QOpenGLFunctions * const _fn;

protected:
  Uniform(QOpenGLFunctions * fn, UniformName id, UniformType type, const GLint addr)
     : _id(id), _addr(addr), _type(type), _fn(fn) {}

public:
  using Ptr = std::shared_ptr<Uniform>;
  static Ptr make(QOpenGLFunctions * fn, UniformName id, UniformType type, const GLint addr) {
    return Ptr(new Uniform(fn, id, type, addr));
  }

  const UniformName &id() const {return _id;}

  // Single scalar
  void setValue(GLfloat v) { _fn->glUniform1f( _addr, v ); }

  void setValue(GLint v) { _fn->glUniform1i( _addr, v ); }

  void setValue(const math::Vector2 &v) {
    _fn->glUniform2fv(_addr, 2, v.elements());
  }

  void setValue(const math::Vector3 &v) {
    _fn->glUniform3fv(_addr, 3, v.elements());
  }

  void setValue(const math::Vector4 &v) {
    _fn->glUniform4fv(_addr, 4, v.elements());
  }

  void setValue(const math::Matrix3 &v) {
    _fn->glUniformMatrix3fv( _addr, 9, GL_FALSE, v.elements());
  }

  void setValue(const math::Matrix4 &v) {
    _fn->glUniformMatrix4fv( _addr, 16, GL_FALSE, v.elements());
  }

// Single texture (2D / Cube)
#if 0
  void setValueT1(const QOpenGLTexture &texture, Renderer_impl &renderer )
  {
    unsigned unit = renderer.allocTextureUnit();
    _fn->glUniform1i( _addr, unit );
    renderer.setTexture2D(v, unit );
  }

  void setValueT6(const QOpenGLTexture &texture, Renderer_impl &renderer )
  {
    unsigned unit = renderer.allocTextureUnit();
    _fn->glUniform1i( _addr, unit );
    renderer.setTextureCube(v, unit );
  }
#endif
  template <unsigned Sz>
  void setValue(const std::array<GLint, Sz> &v) {
    _fn->glUniform2iv(_addr, Sz, v.data());
  }

  virtual Uniform *asUniform() {return this;}
  virtual UniformContainer *asContainer() {return nullptr;}
};

class ArrayUniform : public Uniform
{
  const GLint _index;

protected:
  ArrayUniform(QOpenGLFunctions * fn, UniformName id, UniformType type, const GLint addr)
  : Uniform(fn, id, type, addr), _index(0) {}

public:
  using Ptr = std::shared_ptr<ArrayUniform>;
  static Ptr make(QOpenGLFunctions * fn, UniformName id, UniformType type, const GLint addr) {
    return Ptr(new ArrayUniform(fn, id, type, addr));
  }
};

class UniformContainer
{
  friend class Uniforms;

protected:
  std::vector<UniformName> _sequence;
  std::unordered_map<UniformName, Uniform::Ptr> _map;

public:
  void add(Uniform::Ptr uniform) {
    _sequence.push_back(uniform->id());
    _map[uniform->id()] = uniform;
  }
};

class StructuredUniform : public Uniform, public UniformContainer
{
protected:
  StructuredUniform(QOpenGLFunctions * fn, UniformName id, UniformType type, const GLint addr)
  : Uniform(fn, id, type, addr) {}

public:
  using Ptr = std::shared_ptr<StructuredUniform>;
  static Ptr make(QOpenGLFunctions * fn, UniformName id, UniformType type, const GLint addr) {
    return Ptr(new StructuredUniform(fn, id, type, addr));
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
  QOpenGLFunctions * const _fn;

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

  Uniforms(QOpenGLFunctions * fn, GLuint program) : _fn(fn)
  {
    GLint numUniforms;
    fn->glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &numUniforms);

    for (unsigned i = 0; i < numUniforms; ++ i) {
      parseUniform(program, i, this);
    }
  }

  const std::string _id = "uniforms";

public:
  using Ptr = std::shared_ptr<Uniforms>;
  static Ptr make(QOpenGLFunctions * fn, GLuint program) {
    return Ptr(new Uniforms(fn, program));
  }

  Uniform *get(UniformName name)
  {
    if(_map.find(name) != _map.end()) {
      return _map[name]->asUniform();
    }
  }

  //seqWithValue
  std::vector<Uniform::Ptr> forValues(std::vector<UniformValue> values)
  {
    std::vector<Uniform::Ptr> result;
    for(auto &entry : _map) {

      if(std::find_if(values.begin(), values.end(), [&entry](const UniformValue &val) -> bool {
        return entry.first == val.id;
      }) != values.end()) {
        result.push_back(entry.second);
      }
    }
    return result;
  }
};

}
}
#endif //THREE_QT_UNIFORMS_H
