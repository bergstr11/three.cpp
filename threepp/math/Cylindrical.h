//
// Created by byter on 18.08.17.
//

#ifndef THREE_CPP_CYLINDRICAL_H
#define THREE_CPP_CYLINDRICAL_H

namespace three {
namespace math {

class Cylindrical {
  float _radius, _theta, _y;
  
public:
  Cylindrical(float radius, float theta, float y) : _radius(radius), _theta(theta), _y(y) {}

  Cylindrical &set(float radius, float theta, float y) {

    _radius = radius;
    _theta = theta;
    _y = y;

    return *this;
  }

  static Cylindrical fromVector3(const Vector3 &vec3)
  {
    float radius = std::sqrt(vec3.x() * vec3.x() + vec3.z() * vec3.z());
    float theta = std::atan2( vec3.x(), vec3.z() );
    float y = vec3.y();

    return Cylindrical(radius, theta, y);
  }

  float radius() const {return _radius;}
  float theta() const {return _theta;}
  float y() const {return _y;}
};

}
}
#endif //THREE_CPP_CYLINDRICAL_H
