//
// Created by byter on 5/28/18.
//

#ifndef THREE_PP_ELLIPSE_H
#define THREE_PP_ELLIPSE_H

#include "Curve.h"

namespace three {
namespace extras {
namespace curve {

struct EllipseConfig
{
  float aX = 0.0f, aY = 0.0f;
  float xRadius = 1.0f, yRadius = 1.0f;

  float aStartAngle = 0, aEndAngle = 2 * M_PI;

  bool aClockWise = false;
  float aRotation = 0.0f;
};

class Ellipse : public Curve
{
  EllipseConfig config;

  Ellipse( const EllipseConfig &config) : config(config) {}

  Ellipse( float aX, float aY, float xRadius, float yRadius, float aStartAngle, float aEndAngle,
                bool aClockwise, float aRotation )
  {
    config.aX = aX;
    config.aY = aY;
    config.xRadius = xRadius;
    config.yRadius = yRadius;
    config.aStartAngle = aStartAngle;
    config.aEndAngle = aEndAngle;
    config.aClockWise = aClockwise;
    config.aRotation = aRotation;
  }

  math::Vector2 getPoint( float t )
  {
    float twoPi = M_PI * 2;
    float deltaAngle = config.aEndAngle - config.aStartAngle;
    bool samePoints = std::abs( deltaAngle ) < std::numeric_limits<float>::epsilon();

    // ensures that deltaAngle is 0 .. 2 PI
    while ( deltaAngle < 0 ) deltaAngle += twoPi;
    while ( deltaAngle > twoPi ) deltaAngle -= twoPi;

    if ( deltaAngle < std::numeric_limits<float>::epsilon() ) {

      if ( samePoints ) {

        deltaAngle = 0;
      } else {

        deltaAngle = twoPi;
      }
    }

    if ( config.aClockWise && ! samePoints ) {

      if ( deltaAngle == twoPi ) {

        deltaAngle = - twoPi;
      }
      else {

        deltaAngle = deltaAngle - twoPi;
      }
    }

    float angle = config.aStartAngle + t * deltaAngle;
    float x = config.aX + config.xRadius * std::cos( angle );
    float y = config.aY + config.yRadius * std::sin( angle );

    if ( config.aRotation != 0 ) {

      float cos = std::cos( config.aRotation );
      float sin = std::sin( config.aRotation );

      float tx = x - config.aX;
      float ty = y - config.aY;

      // Rotate the point about the center of the ellipse.
      x = tx * cos - ty * sin + config.aX;
      y = tx * sin + ty * cos + config.aY;
    }

    return math::Vector2( x, y );
  }
};

}
}
}
#endif //THREE_PP_ELLIPSE_H
