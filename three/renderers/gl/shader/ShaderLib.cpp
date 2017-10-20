//
// Created by byter on 18.10.17.
//

#include "ShaderLib.h"

static void qInitResource()
{
  static bool done = false;

  if(done) return;
  done = true;

  //must do this outside of namespace - hence this function
  Q_INIT_RESOURCE(ShaderLib);
  Q_INIT_RESOURCE(ShaderChunk);
}

namespace three {
namespace gl {
namespace shaderlib {

void init() {
  qInitResource();
}

ShaderImpl::ShaderImpl(const UniformValues &uniforms, const char *vertexShader, const char *fragmentShader)
  : Shader(uniforms), _vertex(vertexShader), _fragment(fragmentShader)
{}

const char *ShaderImpl::vertexShader()
{
  if(_vertexShader) return _vertexShader;

  if(_vertex.isCompressed()) {
    _uncompressedVertex = qUncompress(_vertex.data(), _vertex.size());
    _vertexShader = _uncompressedVertex.data();
  }
  else
    _vertexShader = (const char *)_vertex.data();

  return _vertexShader;
}

const char *ShaderImpl::fragmentShader()
{
  if(_fragmentShader) return _fragmentShader;

  if(_fragment.isCompressed()) {
    _uncompressedFragment = qUncompress(_fragment.data(), _fragment.size());
    _fragmentShader = _uncompressedFragment.data();
  }
  else
    _fragmentShader = (const char *)_fragment.data();

  return _fragmentShader;
}
}

}
}
