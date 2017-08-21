//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_SCENE
#define THREEQT_SCENE

#include <Object3D.h>
#include <Color.h>
#include "Fog.h"

namespace three {

class Scene : public Object3D
{
  Color _background;
  Fog _fog;

public:
  Scene(const Color &background, const Fog &fog) : _background(background), _fog(fog) {}
  Scene() {}

  const Color &background() const {return _background;}
  const Fog &fog() const {return _fog;}

  Color &background() {return _background;}
  Fog &fog() {return _fog;}
};

}

#endif //THREEQT_SCENE
