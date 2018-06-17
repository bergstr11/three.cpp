//
// Created by byter on 6/15/18.
//

#include "Interpolations.h"

namespace three {
namespace extras {
namespace interpolations {

using namespace math;

float CatmullRom( float t, float p0, float p1, float p2, float p3 )
{
  float v0 = ( p2 - p0 ) * 0.5f;
  float v1 = ( p3 - p1 ) * 0.5f;
  float t2 = t * t;
  float t3 = t * t2;

  return ( 2 * p1 - 2 * p2 + v0 + v1 ) * t3 + ( - 3 * p1 + 3 * p2 - 2 * v0 - v1 ) * t2 + v0 * t + p1;
}

float QuadraticBezierP0( float t, float p ) {

  float k = 1 - t;
  return k * k * p;
}

float QuadraticBezierP1( float t, float p ) {

  return 2 * ( 1 - t ) * t * p;
}

float QuadraticBezierP2( float t, float p ) {

  return t * t * p;
}

float QuadraticBezier( float t, float p0, float p1, float p2 ) {

  return QuadraticBezierP0( t, p0 ) + QuadraticBezierP1( t, p1 ) + QuadraticBezierP2( t, p2 );
}

float CubicBezierP0( float t, float p ) {

  float k = 1 - t;
  return k * k * k * p;
}

float CubicBezierP1( float t, float p ) {

  float k = 1 - t;
  return 3 * k * k * t * p;
}

float CubicBezierP2( float t, float p ) {

  return 3 * ( 1 - t ) * t * t * p;
}

float CubicBezierP3( float t, float p ) {

  return t * t * t * p;
}

float CubicBezier( float t, float p0, float p1, float p2, float p3 ) {

  return CubicBezierP0( t, p0 ) + CubicBezierP1( t, p1 ) + CubicBezierP2( t, p2 ) +
         CubicBezierP3( t, p3 );
}

}
}
}
