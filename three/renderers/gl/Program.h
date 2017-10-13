//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_GLPROGRAM_H
#define THREE_QT_GLPROGRAM_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <Constants.h>
#include <QOpenGLFunctions>
#include "Helpers.h"
#include "Extensions.h"
#include "Uniforms.h"

namespace three {
namespace gl {

std::vector<std::string> getEncodingComponents(Encoding encoding);

std::string getTexelDecodingFunction(const char *functionName, Encoding encoding);

std::string getTexelEncodingFunction(const char *functionName, Encoding encoding );

std::string getToneMappingFunction(const char *functionName, ToneMapping toneMapping);

std::string generateDefines(std::unordered_map<std::string, std::string> defines);

std::string generateExtensions(const Extensions &extensions, const UseExtension &use, const Parameters &parameters);


class Program : private QOpenGLFunctions
{
  static unsigned programIdCount;

  unsigned _id;

  GLuint _program;

  Renderer_impl &_renderer;

  Uniforms::Ptr _cachedUniforms;

  std::unordered_map<std::string, GLint> fetchAttributeLocations();

public:
  using Ptr = std::shared_ptr<Program>;

  unsigned id() const {return _id;}

  Uniforms::Ptr getUniforms()
  {
    if (_cachedUniforms == nullptr) {
      _cachedUniforms = Uniforms::make(this, _program);
    }

    return _cachedUniforms;
  }
};

}
}
#endif //THREE_QT_GLPROGRAM_H
