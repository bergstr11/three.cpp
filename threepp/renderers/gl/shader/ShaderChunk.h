//
// Created by byter on 30.09.17.
//

#ifndef THREEPP_SHADERCHUNK_H
#define THREEPP_SHADERCHUNK_H

#include <string>

namespace three {
namespace gl {

enum class ShaderChunk
{
  tonemapping_pars_fragment,
  encodings_pars_fragment
};

const char *getShaderChunk(ShaderChunk chunk);

const char *getShaderChunk(std::string chunk);

}
}
#endif //THREEPP_SHADERCHUNK_H
