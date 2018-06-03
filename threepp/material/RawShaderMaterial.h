//
// Created by byter on 10/31/17.
//

#ifndef THREEPP_RAWSHADERMATERIAL_H
#define THREEPP_RAWSHADERMATERIAL_H

#include "ShaderMaterial.h"

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 */
class DLX RawShaderMaterial : public ShaderMaterial
{
  RawShaderMaterial(const gl::UniformValues &uniforms,
                    const char *vertexShader,
                    const char *fragmentShader,
                    Side side, bool depthTest, bool depthWrite, bool fog)
  : ShaderMaterial(material::InfoT<RawShaderMaterial>(),
                   material::Typer(this).allow<ShaderMaterial>(),
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
#endif //THREEPP_RAWSHADERMATERIAL_H
