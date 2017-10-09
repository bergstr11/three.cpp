//
// Created by byter on 09.10.17.
//

#ifndef THREE_QT_UNIFORMS_H
#define THREE_QT_UNIFORMS_H

#include <QOpenGLFunctions>
#include <array>
#include <unordered_map>

namespace three {
namespace gl {

/**
 * @author tschw
 *
 * Uniforms of a program.
 * Those form a tree structure with a special top-level container for the root,
 * which you get by calling 'new WebGLUniforms( gl, program, renderer )'.
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
 * .setValue( gl, value, [renderer] )
 *
 * 		uploads a uniform value(s)
 *  	the 'renderer' parameter is needed for sampler uniforms
 *
 *
 * Static methods of the top-level container (renderer factorizations):
 *
 * .upload( gl, seq, values, renderer )
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
 * .setValue( gl, name, value )
 *
 * 		sets uniform with  name 'name' to 'value'
 *
 * .set( gl, obj, prop )
 *
 * 		sets uniform from object and property with same name than uniform
 *
 * .setOptional( gl, obj, prop )
 *
 * 		like .set for an optional property of the object
 *
 */
class Uniforms
{
  QOpenGLFunctions * _fn;

  // --- Utilities ---

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
  std::vector<int32_t> &allocTexUnits(Renderer_impl &renderer, size_t n)
  {
    if(arrayCacheI32.find(n) == arrayCacheI32.end()) {
      arrayCacheI32.emplace(n, std::vector<int32_t>(n));
      arrayCacheI32[n].resize(n);
    }
    auto &r = arrayCacheI32[ n ];

    for ( size_t i = 0; i < n; ++i)
      r[i] = renderer.allocTextureUnit();

    return r;
  }

};

}
};
#endif //THREE_QT_UNIFORMS_H
