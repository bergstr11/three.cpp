//
// Created by byter on 03.09.17.
//

#ifndef THREE_QT_SPRITEMATERIAL_H
#define THREE_QT_SPRITEMATERIAL_H

#include "Material.h"

namespace three {

class SpriteMaterial : public Material
{
  SpriteMaterial() : Material(false, false, false) {}
public:
  using Ptr = std::shared_ptr<SpriteMaterial>;
  static Ptr make() {return Ptr(new SpriteMaterial());}
};
}
#endif //THREE_QT_SPRITEMATERIAL_H
