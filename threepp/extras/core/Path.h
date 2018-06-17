#ifndef THREEPP_EXTRAS_PATH
#define THREEPP_EXTRAS_PATH

#include <threepp/math/Vector2.h>
#include <threepp/extras/core/CurvePath.h>
#include <threepp/extras/curve/Ellipse.h>
#include <threepp/extras/curve/Spline.h>
#include <threepp/extras/curve/CubicBezier.h>
#include <threepp/extras/curve/QuadraticBezier.h>
#include <threepp/extras/curve/Line.h>

namespace three {
namespace extras {

/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 * Creates free form 2d path using series of points, lines or curves.
 **/
class Path : public CurvePath
{
protected:
	math::Vector2 currentPoint;

	Path(const std::vector<math::Vector2> &points)
  {
    setFromPoints( points );
  }

  Path(const std::vector<Curve::Ptr> &curves) : CurvePath(curves) {}

public:
  using Ptr = std::shared_ptr<Path>;

	static Ptr make(const std::vector<math::Vector2> &points) {
	  return Ptr(new Path(points));
	}

  static Ptr make(const std::vector<Curve::Ptr> &curves) {
    return Ptr(new Path(curves));
  }

	Path &setFromPoints( const std::vector<math::Vector2> &points )
  {
		moveTo( points[ 0 ].x(), points[ 0 ].y() );

		for ( const auto &point: points) {

			lineTo( point.x(), point.y() );
		}
		return *this;
	}

	Path &moveTo( float x, float y ) {

		currentPoint.set( x, y );
		return *this;
	}

	Path &lineTo( float x, float y ) {

		Curve::Ptr curve = curve::Line::make( currentPoint, math::Vector2( x, y ) );
		_curves.push_back( curve );

		currentPoint.set( x, y );
    return *this;
	}

	Path &quadraticCurveTo( float aCPx, float aCPy, float aX, float aY ) {

		Curve::Ptr curve = curve::QuadraticBezier::make(currentPoint,
                                                    math::Vector2( aCPx, aCPy ),
                                                    math::Vector2( aX, aY ));
		_curves.push_back( curve );

		currentPoint.set( aX, aY );
    return *this;
	}

	Path &bezierCurveTo( float aCP1x, float aCP1y, float aCP2x, float aCP2y, float aX, float aY )
  {
		Curve::Ptr curve = curve::CubicBezier::make(
			currentPoint,
			math::Vector2( aCP1x, aCP1y ),
      math::Vector2( aCP2x, aCP2y ),
      math::Vector2( aX, aY )
		);

		_curves.push_back( curve );
		currentPoint.set( aX, aY );

    return *this;
	}

	Path &splineThru( const std::vector<math::Vector2> &pts )
  {
    std::vector<math::Vector2> npts;
    npts.push_back(currentPoint);
    npts.insert(npts.end(), pts.begin(), pts.end());

		Curve::Ptr curve = curve::Spline::make( npts );
		_curves.push_back( curve );

		currentPoint = pts.back();

    return *this;
	}

	Path &arc( float aX, float aY, float aRadius, float aStartAngle, float aEndAngle, bool aClockwise )
  {
    float x0 = currentPoint.x();
    float y0 = currentPoint.y();

		absarc( aX + x0, aY + y0, aRadius, aStartAngle, aEndAngle, aClockwise );

    return *this;
	}

	Path &absarc( float aX, float aY, float aRadius, float aStartAngle, float aEndAngle, bool aClockwise )
  {
		absellipse( aX, aY, aRadius, aRadius, aStartAngle, aEndAngle, aClockwise, 0 );

    return *this;
	}

	Path &ellipse( float aX, float aY, float xRadius, float yRadius, float aStartAngle, float aEndAngle,
                 bool aClockwise, float aRotation ) {

    float x0 = currentPoint.x();
    float y0 = currentPoint.y();

		absellipse( aX + x0, aY + y0, xRadius, yRadius, aStartAngle, aEndAngle, aClockwise, aRotation );

    return *this;
	}

	Path &absellipse( float aX, float aY, float xRadius, float yRadius, float aStartAngle, float aEndAngle,
                    bool aClockwise, float aRotation )
  {
		Curve::Ptr curve = curve::Ellipse::make( curve::EllipseConfig(aX, aY, xRadius, yRadius,
                                                                  aStartAngle, aEndAngle,
                                                                  aClockwise, aRotation) );

		if ( !_curves.empty() ) {

			// if a previous curve is present, attempt to join
			math::Vector2 firstPoint = curve->getPoint( 0 );

			if ( firstPoint != currentPoint ) {

				lineTo( firstPoint.x(), firstPoint.y() );
			}
		}

		_curves.push_back( curve );

		currentPoint = curve->getPoint( 1 );

    return *this;
	}
};

}
}

#endif //THREEPP_EXTRAS_PATH
