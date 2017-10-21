//
// Created by byter on 11.08.17.
//

#ifndef THREE_QT_VECTOR2_H
#define THREE_QT_VECTOR2_H

#include <cassert>
#include <algorithm>
#include <cmath>

#include <helper/UV.h>

namespace three {

template<typename T>
class BufferAttributeBase;

namespace math {

class Vector2
{
  union {
    struct {
      float _x, _y;
    };
    float _elements[2];
  };

public:
  Vector2() : _x(0.0f), _y(0.0f) {}

  Vector2(float x, float y) : _x(x), _y(y) {}

  Vector2(const UV &uv) : _x(uv.u()), _y(uv.v()) {}

  Vector2(const Vector2 &vector) : _x(vector._x), _y(vector._y) {}

  Vector2(float scalar) : _x(scalar), _y(scalar) {}

  static Vector2 fromBufferAttribute(const BufferAttributeBase<float> &attribute, unsigned index);

  const float x() const { return _x; }
  const float y() const { return _y; }
  const float width() const { return _x; }
  const float height() const { return _y; }
  float &x() { return _x; }
  float &y() { return _y; }

  const float *elements() const {return _elements;}

  const float length() const
  {
    return std::sqrt(_x * _x + _y * _y );
  }

  const float lengthSq() const
  {
    return _x * _x + _y * _y;
  }

  const float manhattanLength() const
  {
    return std::abs(_x) + std::abs(_y);
  }

  float &operator[](unsigned index)
  {
    assert(index < 2);
    return _elements[index];
  }

  const float operator[] (unsigned index) const
  {
    assert(index < 2);
    return _elements[index];
  }

  Vector2 &operator+=(const Vector2 &vector)
  {
    _x += vector._x;
    _y += vector._y;
    return *this;
  }

  Vector2 &operator-=(const Vector2 &vector)
  {
    _x -= vector._x;
    _y -= vector._y;
    return *this;
  }

  Vector2 &operator*=(const Vector2 &vector)
  {
    _x *= vector._x;
    _y *= vector._y;
    return *this;
  }

  Vector2 &operator/=(const Vector2 &vector)
  {
    _x /= vector._x;
    _y /= vector._y;
    return *this;
  }

  Vector2 &operator += (float scalar)
  {
    _x += scalar;
    _y += scalar;
    return *this;
  }

  Vector2 &operator -= (float scalar)
  {
    _x -= scalar;
    _y -= scalar;
    return *this;
  }

  Vector2 &operator *= (float scalar)
  {
    _x *= scalar;
    _y *= scalar;
    return *this;
  }

  Vector2 &operator /= (float scalar)
  {
    _x /= scalar;
    _y /= scalar;
    return *this;
  }

  Vector2 &clamp(const Vector2 &min, const Vector2 &max)
  {
    // assumes min < max, componentwise
    _x = std::max(min.x(), std::min(max.x(), _x));
    _y = std::max(min.y(), std::min(max.y(), _y));
    return *this;
  }

  Vector2 &clampScalar(float min, float max) {

    Vector2 vmin {min, min};
    Vector2 vmax {max, max};

    return clamp(vmin, vmax);
  }

  Vector2 &clampLength(float min, float max)
  {
    float len = length();
    *this /= (len > 0 ? len : 1);
    *this *= std::max(min, std::min(max, len));
    return *this;
  }

  Vector2 &floor()
  {
    _x = std::floor(_x);
    _y = std::floor(_y);

    return *this;
  }

  Vector2 &ceil()
  {
    _x = std::ceil(_x);
    _y = std::ceil(_y);
    return *this;
  }

  Vector2 &round()
  {
    _x = std::round(_x);
    _y = std::round(_y);
    return *this;
  }

  Vector2 &roundToZero()
  {
    _x = _x < 0 ? std::ceil(_x) : std::floor(_x);
    _y = _y < 0 ? std::ceil(_y) : std::floor(_y);
    return *this;
  }

  float dot(const Vector2 &vector) const
  {
    return _x * vector._x + _y * vector._y;
  }

  Vector2 &normalize()
  {
    float len = length();
    return *this /= len > 0 ? len : 1;
  }

  // computes the angle in radians with respect to the positive x-axis
  float angle()
  {
    float angle = std::atan2(_y, _x);
    if(angle < 0) angle += 2 * M_PI;
    return angle;
  }

  float squaredDistance(const Vector2 &vector)
  {
    float dx = _x - vector._x, dy = _y - vector._y;
    return dx * dx + dy * dy;
  }

  float distanceTo(const Vector2 &vector)
  {
    return std::sqrt(squaredDistance(vector));
  }

  float manhattanDistance(const Vector2 &vector)
  {
    return std::abs(_x - vector._x) + std::abs(_y - vector._y);
  }

  Vector2 &setLength(float length)
  {
    return normalize() *= length;
  }

  Vector2 &lerp(const Vector2 &vector, float alpha)
  {
    _x += (vector._x - _x) * alpha;
    _y += (vector._y - _y) * alpha;

    return *this;
  }

  Vector2 &lerpVectors(const Vector2 &v1, const Vector2 &v2, float alpha)
  {
    _x = v1._x - v2._x;
    _y = v1._y - v2._y;

    return (*this *= alpha) += v1;

  }

  bool operator == (const Vector2 &vector) const
  {
    return vector._x == _x && vector._y == _y;
  }

  Vector2(const float *array, unsigned offset=0)
  {
    _x = array[offset];
    _y = array[offset + 1];
  }

  void put(float *array, unsigned offset=0)
  {
    array[offset] = _x;
    array[offset + 1] = _y;
  }

  Vector2 &rotateAround(const Vector2 &center, float angle)
  {
    float c = std::cos(angle), s = std::sin(angle);

    float x = _x - center._x;
    float y = _y - center._y;

    _x = x * c - y * s + center._x;
    _y = x * s + y * c + center._y;

    return *this;
  }
};

inline Vector2 operator + (const Vector2 &left, const Vector2 &right)
{
  Vector2 result {left};
  result += right;
  return result;
}

inline Vector2 operator -(const Vector2 &left, const Vector2 &right)
{
  Vector2 result {left};
  result -= right;
  return result;
}

inline Vector2 operator *(const Vector2 &left, const Vector2 &right)
{
  Vector2 result {left};
  result *= right;
  return result;
}

inline Vector2 operator *(const Vector2 &left, float right)
{
  Vector2 result {left};
  result *= right;
  return result;
}

inline Vector2 min(const Vector2 &v1, const Vector2 &v2) {
  return Vector2(std::min(v1.x(), v2.x()), std::min(v1.y(), v2.y()));
}

inline Vector2 max(const Vector2 &v1, const Vector2 &v2) {
  return Vector2(std::max(v1.x(), v2.x()), std::max(v1.y(), v2.y()));
}

} //math
} //three

#endif //THREE_QT_VECTOR2_H
