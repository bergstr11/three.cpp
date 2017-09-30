//
// Created by byter on 29.09.17.
//

#ifndef THREE_QT_CUBETEXTURE_H
#define THREE_QT_CUBETEXTURE_H

#include "Texture.h"

namespace three {

class CubeTexture :public Texture
{
protected:
  CubeTexture(const Options &data) : Texture(data)
  {
    _flipY = false;
  }

public:
  using Ptr = std::shared_ptr<CubeTexture>;
  static Ptr make(const Options &options) {
    return Ptr(new CubeTexture(options));
  }

};

}
#endif //THREE_QT_CUBETEXTURE_H
