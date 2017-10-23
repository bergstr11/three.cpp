//
// Created by byter on 03.09.17.
//

#ifndef THREE_QT_SPRITEMATERIAL_H
#define THREE_QT_SPRITEMATERIAL_H

#include "Material.h"

namespace three {

class SpriteMaterial : public Material
{
  Color _color = {255, 255, 255};
  Texture::Ptr _map = nullptr;

  float _rotation = 0;

  SpriteMaterial()
  {
    this->_morphTargets = false;
    this->_morphNormals = false;
    this->_skinning = false;
  }
public:
  using Ptr = std::shared_ptr<SpriteMaterial>;
  static Ptr make() {return Ptr(new SpriteMaterial());}

  bool fog = false;
  bool lights = false;
  
  const Color &color() const {return _color;}
  const Texture::Ptr map() const {return _map;}

  float rotation() const {return _rotation;}
};
}
#endif //THREE_QT_SPRITEMATERIAL_H
