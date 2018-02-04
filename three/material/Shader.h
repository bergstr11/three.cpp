//
// Created by byter on 11/17/17.
//

#ifndef THREE_QT_SHADER_H
#define THREE_QT_SHADER_H

#include <string>
#include <three/renderers/gl/shader/ShaderLib.h>

namespace three {

/**
 * represents a OpenGL shader program. This class is only defined at the non-renderer level to
 * accomodate the needs of ShaderMaterial
 */
class Shader
{
protected:
  const char * _name;
  gl::UniformValues _uniforms;

  std::string _vertexShader;
  std::string _fragmentShader;

public:
  Shader()
     : _name(nullptr), _uniforms({}) {}

  Shader(const char *name, const gl::UniformValues &uniforms, const std::string &vertexShader, const std::string &fragmentShader)
     : _name(name), _uniforms(uniforms), _vertexShader(vertexShader), _fragmentShader(fragmentShader) {}

  Shader(const char *name, const gl::ShaderInfo &shaderInfo)
     : _name(name), _uniforms(shaderInfo.uniforms), _vertexShader(shaderInfo.vertexShader), _fragmentShader(shaderInfo.fragmentShader) {}

  gl::UniformValues &uniforms() {return _uniforms;}

  const char *vertexShader() {return _vertexShader.data();}
  const char *fragmentShader() {return _fragmentShader.data();}

  const char *name() const {return _name;}
};

}

#endif //THREE_QT_SHADER_H
