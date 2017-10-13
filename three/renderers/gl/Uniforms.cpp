//
// Created by byter on 12.10.17.
//

#include "Uniforms.h"
#include "Renderer_impl.h"
#include <regex>

namespace three {
namespace gl {

using namespace std;

std::vector<int32_t> &Uniforms::allocTexUnits(Renderer_impl &renderer, size_t n)
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

void Uniforms::parseUniform(GLuint program, unsigned index, UniformContainer *container)
{
  static regex rex(R"(([\w\d_]+)(\])?(\[|\.)?)");

  GLsizei bufSize = 100;
  GLchar uname[100];
  GLsizei *length;
  GLint *size;
  GLenum *type;

  container->fn()->glGetActiveUniform( program, index, bufSize, length, size, type, uname);
  GLint addr = container->fn()->glGetUniformLocation(program, uname);

  string name(uname);
  sregex_iterator rex_it(name.cbegin(), name.cend(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    smatch match = *rex_it;

    string id = match[1];
    bool isIndex = match[2] == "]";
    string subscript = match[3];

    if(!match[3].matched || match[3].second == name.end()) {
      // bare name or "pure" bottom-level array "[0]" suffix
      container->add(match[3].matched ?
          Uniform::make(container->fn(), id, (UniformType)*type, addr) :
          ArrayUniform::make(container->fn(), id, (UniformType)*type, addr));
    }
    else {
      // step into inner node / create it in case it doesn't exist
      if(container->_map.find(id) == container->_map.end()) {
        StructuredUniform::Ptr next = StructuredUniform::make(container->fn(), id, (UniformType)*type, addr);
        container->add(next);
      }
      container = container->_map[id]->asContainer();
      assert(container != nullptr);
    }
    rex_it++;
  }
}

}
}