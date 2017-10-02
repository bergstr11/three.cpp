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

namespace three {
namespace gl {

struct Extensions {
  bool derivatives;
  bool fragDepth;
  bool drawBuffers;
  bool shaderTextureLOD;
};

struct RenderExtensions {
  bool frag_depth;          //EXT_frag_depth
  bool draw_buffers;        //WEBGL_draw_buffers
  bool shader_texture_lod;  //EXT_shader_texture_lod
};

std::vector<std::string> getEncodingComponents(Encoding encoding);

std::string getTexelDecodingFunction(const char *functionName, Encoding encoding);

std::string getTexelEncodingFunction(const char *functionName, Encoding encoding );

std::string getToneMappingFunction(const char *functionName, ToneMapping toneMapping);

std::string generateDefines(std::unordered_map<std::string, std::string> defines);

std::string generateExtensions(const Extensions &extensions, const Parameters &parameters, const RenderExtensions &rendererExtensions);


class Program : private QOpenGLFunctions
{
  static unsigned programIdCount = 0;

  unsigned _id;

  GLuint _program;

  std::unordered_map<std::string, GLint> fetchAttributeLocations();

public:
  using Ptr = std::shared_ptr<Program>;

  unsigned id() const {return _id;}
};

}
}
#endif //THREE_QT_GLPROGRAM_H
