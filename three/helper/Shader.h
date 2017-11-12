//
// Created by byter on 19.10.17.
//

#ifndef THREE_QT_UNIFORMVALUE_H
#define THREE_QT_UNIFORMVALUE_H

#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Vector4.h>
#include <math/Matrix3.h>
#include <math/Matrix4.h>
#include <core/Color.h>
#include <textures/CubeTexture.h>
#include <unordered_map>
#include <initializer_list>
#include <renderers/gl/shader/ShaderLib.h>

namespace three {

class Shader
{
protected:
  const char * _name;
  gl::UniformValues _uniforms;

  std::string _vertexShader;
  std::string _fragmentShader;

public:
  Shader()
     : _name(nullptr), _uniforms({}), _vertexShader(nullptr), _fragmentShader(nullptr) {}

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

#endif //THREE_QT_UNIFORMVALUE_H
