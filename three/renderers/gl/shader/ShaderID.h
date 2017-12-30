//
// Created by byter on 11/19/17.
//

#ifndef THREE_QT_SHADERID_H
#define THREE_QT_SHADERID_H

#include <string>

namespace three {
namespace gl {

enum class ShaderID : unsigned
{
  basic=0,
  lambert=1,
  phong=2,
  standard=3,
  normal=4,
  points=5,
  dashed=6,
  depth=7,
  cube=8,
  equirect=9,
  distanceRGBA=10,
  shadow=11,
  physical=12,

  undefined=999
};

ShaderID toShaderID(std::string name);

}
}
#endif //THREE_QT_SHADERID_H
