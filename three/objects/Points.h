//
// Created by byter on 27.09.17.
//

#ifndef THREE_QT_POINTS_H
#define THREE_QT_POINTS_H

#include <core/Object3D.h>

namespace three {

class Points : public Object3D
{
public:
  bool renderable() const override {return true;}
};

}
#endif //THREE_QT_POINTS_H
