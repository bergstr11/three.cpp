//
// Created by byter on 18.10.17.
//

#include "ShaderLib.h"
#include <utility>

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

class Shader;

class LibShader : public three::Shader
{
  friend class three::gl::Shader;

  ShaderID _id;
  QResource _vertex;
  QByteArray _uncompressedVertex;
  const char *_vertexShader = nullptr;

  QResource _fragment;
  QByteArray _uncompressedFragment;
  const char *_fragmentShader = nullptr;

public:
  LibShader(ShaderID id, const UniformValues &uniforms, const char *vertexShader, const char *fragmentShader)
     : three::Shader("", uniforms), _id(id), _vertex(vertexShader), _fragment(fragmentShader)
  {}

  LibShader(const LibShader &shader)
     : three::Shader("", shader._uniforms), _id(shader._id), _vertex(shader._vertexShader), _fragment(shader._fragmentShader)
  {}

  const std::string vertexShader() override
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

  const std::string fragmentShader() override
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
};

class Shader : public three::Shader
{
  LibShader & _libShader;

public:
  Shader(const char *name, LibShader &libShader) : three::Shader(name, _libShader._uniforms), _libShader(libShader) {}

  const std::string vertexShader() override
  {
    return _libShader.vertexShader();
  }
  const std::string fragmentShader() override
  {
    return _libShader.fragmentShader();
  }
};

class Shaders
{
  std::unordered_map<ShaderID, three::gl::LibShader> _shaders;

public:
  Shaders() {
    qInitResource();

    _shaders.emplace(std::make_pair(ShaderID::cube, LibShader(ShaderID::cube,
       {
          uniform::Cube(nullptr),
          uniform::Flip(-1),
          uniform::Opacity(1.0f)
       },
       ":chunk/cube_vert.glsl",
       ":chunk/cube_frag.glsl"
    )));
  }
  LibShader &operator[](ShaderID id) {
    return _shaders.at(id);
  }
};

namespace shaderlib {

three::Shader::Ptr get(ShaderID id, const char *name)
{
  static Shaders shaders;

  return three::Shader::Ptr(new three::gl::Shader(name, shaders[id]));
}

three::Shader &get(ShaderID id)
{
  static Shaders shaders;

  return shaders[id];
}

}

}
}
