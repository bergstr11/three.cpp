//
// Created by byter on 10/31/17.
//

#ifndef THREE_QT_RAWSHADERMATERIAL_H
#define THREE_QT_RAWSHADERMATERIAL_H

#include "ShaderMaterial.h"

namespace three {
namespace gl {

class RawShaderMaterial : public ShaderMaterial
{
  RawShaderMaterial(Shader &shader, Side side, bool depthTest, bool depthWrite, bool fog)
  : ShaderMaterial(material::ResolverT<RawShaderMaterial>::make(*this), shader, side, depthTest, depthWrite, fog)
  {}


public:
  using Ptr = std::shared_ptr<RawShaderMaterial>;

  static Ptr make(Shader &shader, Side side, bool depthTest, bool depthWrite, bool fog) {
    return Ptr(new RawShaderMaterial(shader, side, depthTest, depthWrite, fog));
  }
};

}
}
#endif //THREE_QT_RAWSHADERMATERIAL_H
