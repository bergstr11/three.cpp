//
// Created by byter on 10/31/17.
//

#ifndef THREE_QT_RAWSHADERMATERIAL_H
#define THREE_QT_RAWSHADERMATERIAL_H

#include "ShaderMaterial.h"

namespace three {

class RawShaderMaterial : public ShaderMaterial
{
  RawShaderMaterial(const gl::UniformValues &uniforms,
                    const char *vertexShader,
                    const char *fragmentShader,
                    Side side, bool depthTest, bool depthWrite, bool fog)
  : ShaderMaterial(material::ResolverT<RawShaderMaterial>::make(*this),
                   uniforms, vertexShader, fragmentShader, side, depthTest, depthWrite, fog)
  {}


public:
  using Ptr = std::shared_ptr<RawShaderMaterial>;

  static Ptr make(const gl::UniformValues &uniforms,
                  const char *vertexShader,
                  const char *fragmentShader,
                  Side side, bool depthTest, bool depthWrite, bool fog) {
    return Ptr(new RawShaderMaterial(uniforms, vertexShader, fragmentShader, side, depthTest, depthWrite, fog));
  }
};

}
#endif //THREE_QT_RAWSHADERMATERIAL_H
