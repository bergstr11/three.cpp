//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_SHADER_H
#define THREE_QT_SHADER_H

#include <QOpenGLFunctions>
#include <Constants.h>
#include <iostream>
#include <stdexcept>

namespace three {
namespace gl {

class LibShader : private QOpenGLFunctions
{
  GLuint _shader;

public:
  LibShader(ShaderType type, std::string glsl)
  {
    _shader = glCreateShader((GLenum)type);

    GLint len = glsl.length();
    const char *source = glsl.data();
    glShaderSource(_shader, 1, &source, &len);

    glCompileShader(_shader);

    GLint status;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
    bool compiled = status == GL_TRUE;

    GLchar infoLog[513];
    glGetShaderInfoLog(_shader, 512, &len, infoLog);
    if(len > 0) {
      infoLog[len] = 0;
      std::string log(infoLog);
      std::cerr << "three::gl::Shader" << (type == ShaderType::Vertex  ? "(vertex)" : "(fragment)")
                << " compile log:" << std::endl << log << std::endl;
      std::cerr << "shader source:" << std::endl << glsl;
    }

    // --enable-privileged-webgl-extension
    // console.log( type, gl.getExtension( 'WEBGL_debug_shaders' ).getTranslatedShaderSource( shader ) );
    if(!compiled)
      throw std::logic_error("shader compile error");
  }
};

}
}
#endif //THREE_QT_SHADER_H
