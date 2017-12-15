//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_AXES_H
#define THREEPP_AXES_H

#include "quick/ThreeDScene.h"
#include <helper/AxesHelper.h>

namespace three {
namespace quick {

class Axes : public ThreeDObject
{
  Q_OBJECT
  Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)

  size_t _size = 1;

  helper::AxesHelper::Ptr _axes;

public:
  three::Object3D::Ptr create() override
  {
    _axes = helper::AxesHelper::make("axes", _size);

    return _axes;
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
