//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_AXES_H
#define THREEPP_AXES_H

#include "ThreeDScene.h"
#include <helper/AxesHelper.h>

namespace three {
namespace quick {

class Axes : public ThreeDObject
{
  Q_OBJECT
  Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)

  size_t _size = 1;

public:
  void addTo(three::Scene::Ptr scene) override
  {
    helper::AxesHelper::Ptr axes = helper::AxesHelper::make("axis", _size);

    scene->add(axes);
  }

  size_t size() {return _size;}

  void setSize(size_t size) {
    if(_size != size) {
      _size = size;
      emit sizeChanged();
    }
  }

signals:
  void sizeChanged();
};

}
}

#endif //THREEPP_AXES_H
