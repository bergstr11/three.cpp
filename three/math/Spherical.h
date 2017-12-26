//
// Created by byter on 13.08.17.
//

#ifndef THREE_QT_SPHERICAL_H
#define THREE_QT_SPHERICAL_H

#include <algorithm>
#include <cmath>
#include "Vector3.h"

namespace three {
namespace math {

class Spherical {
  float _radius, _phi, _theta;

public:
  Spherical() : _radius(1.0), _phi(0), _theta(0) {}
  Spherical(float radius, float phi=0, float theta=0) : _radius(radius), _phi(phi), _theta(theta) {}
  Spherical(const Spherical &spherical) : _radius(spherical._radius), _phi(spherical._phi), _theta(spherical._theta) {}

  float phi() const {return _phi;}
  float &phi() {return _phi;}

  float theta() const {return _theta;}
  float &theta() {return _theta;}

  float radius() const {return _radius;}
  float &radius() {return _radius;}

  // restrict phi to be betwee EPS and PI-EPS
  Spherical &makeSafe()
  {
    float EPS = 0.000001;
    _phi = std::max( EPS, (float)std::min(M_PI - EPS, (double)_phi));

    return *this;
  }

  Spherical &setFromVector3(const Vector3 &vec3 )
  {
    _radius = vec3.length();

    if ( _radius == 0 ) {
      _theta = 0;
      _phi = 0;
    }
    else {
      _theta = std::atan2(vec3.x(), vec3.z()); // equator angle around y-up axis
      _phi = std::acos(math::clamp( vec3.y() / _radius, -1.0f, 1.0f) ); // polar angle
    }

    return *this;
  }

  Spherical &set(float radius, float phi, float theta)
  {
    _radius = radius;
    _phi = phi;
    _theta = theta;

    return *this;
  }
};

}
}
#endif //THREE_QT_SPHERICAL_H
