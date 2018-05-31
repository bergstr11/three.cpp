//
// Created by byter on 30.07.17.
//

#ifndef THREEPP_MATH_H
#define THREEPP_MATH_H

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#include <cmath>
#include <algorithm>
#include <threepp/math/Matrix3.h>

namespace three {
namespace math {

class Matrix4;
class Vector3;
class Quaternion;
class Vector3;

static const float EPS = 0.000001f;

static const double DEG2RAD = M_PI / 180;
static const double RAD2DEG = 180 / M_PI;

template <typename T> T clamp(T value, T min, T max)
{
  return std::max(min, std::min(max, value));
}

// compute euclidian modulo of m % n
// https://en.wikipedia.org/wiki/Modulo_operation
template <typename T> T euclideanModulo( T n, T m)
{
  return (( n % m ) + m) % m;
}

// Linear mapping from range <a1, a2> to range <b1, b2>
template <typename T> T mapLinear(T x, T a1, T a2, T b1, T b2 ) 
{
  return b1 + ( x - a1 ) * ( b2 - b1 ) / ( a2 - a1 );
}

// https://en.wikipedia.org/wiki/Linear_interpolation
template <typename T> T lerp(T x, T y, T t) {

  return ( 1 - t ) * x + t * y;

}

// http://en.wikipedia.org/wiki/Smoothstep
template <typename T> T smoothstep(T x, T min, T max ) {
  
  if ( x <= min ) return 0;
  if ( x >= max ) return 1;

  x = ( x - min ) / ( max - min );

  return x * x * ( 3 - 2 * x );
}

template <typename T> T smootherstep(T x, T min, T max )
{
  if ( x <= min ) return 0;
  if ( x >= max ) return 1;

  x = ( x - min ) / ( max - min );

  return x * x * x * ( x * ( x * 6 - 15 ) + 10 );
}

// Random integer from <low, high> interval
inline int rand(int low, int high)
{
  return low + (int)std::floor(std::rand() * (high - low + 1));
}

// Random float from <low, high> interval

inline float rand(float low, float high)
{
  return low + std::rand() * ( high - low );
}

// Random float from <-range/2, range/2> interval
inline float randSpread(float range) {

  return range * ( 0.5f - std::rand() );
}

inline double degToRad(unsigned short degrees)
{
  return degrees * DEG2RAD;
}

inline unsigned short radToDeg(double radians)
{
  return (unsigned short)(radians * RAD2DEG);
}

inline bool isPowerOfTwo(int value )
{
  return (value & (value - 1)) == 0 && value != 0;
}

inline int nearestPowerOfTwo(int value)
{
  return (int)std::pow( 2, round( std::log( value ) / M_LN2 ) );
}

inline int ceilPowerOfTwo(float value)
{
  return (int)std::pow( 2, std::ceil( std::log( value ) / M_LN2 ) );
}

inline int floorPowerOfTwo(float value)
{
  return (int)std::pow( 2, std::floor( std::log( value ) / M_LN2 ) );
}

inline int nextPowerOfTwo(int value )
{
  value --;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value ++;

  return value;
}

void decompose(const Matrix4 &matrix, Vector3 &position, Quaternion &quaternion, Vector3 &scale );

}
}
#endif //THREEPP_MATH_H
