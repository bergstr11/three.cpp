//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_EULER
#define THREEQT_EULER

#include "math/Matrix4.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"
#include "helper/simplesignal.h"

namespace three {
namespace math {

class Euler
{
public:
  enum RotationOrder {XYZ, YZX, ZXY, XZY, YXZ, ZYX, Default};

private:
  float _x;
  float _y;
  float _z;
  RotationOrder _order;

public:
  Euler(float x, float y, float z, RotationOrder order) : _x(x), _y(y), _z(z), _order(order) {}

  Euler() : _x(0), _y(0), _z(0), _order(XYZ) {}

  Euler(const Euler &euler) : _x(euler._x), _y(euler._y), _z(euler._z), _order(euler._order) {}

  Euler(math::Quaternion q, RotationOrder order) {
    set(q, order);
  }

  Signal<void(const Euler &)> onChange;

  Euler & operator = (const Euler &other);

  void set(float x, float y, float z, RotationOrder order, bool emitSignal=true);

  void set(const math::Matrix4 &m, RotationOrder order, bool emitSignal=true);
  void set(const math::Quaternion &q, RotationOrder order, bool emitSignal=true);
  void set(const math::Vector3 &v, RotationOrder order, bool emitSignal=true);
  void reorder(RotationOrder order);

  math::Quaternion toQuaternion() const;
  RotationOrder getRotationOrder() const {return _order;}
  bool operator ==(const Euler &other);

  const float x() const {return _x;}
  const float y() const {return _y;}
  const float z() const {return _z;}

  void setX(float x) {
    _x = x;
    onChange.emitSignal(*this);
  }
  void setY(float y) {
    _y = y;
    onChange.emitSignal(*this);
  }
  void setZ(float z) {
    _z = z;
    onChange.emitSignal(*this);
  }

  const RotationOrder order() const {return _order;}
};

}
}

#endif //THREEQT_EULER
