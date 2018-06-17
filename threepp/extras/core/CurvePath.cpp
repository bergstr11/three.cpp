//
// Created by byter on 6/15/18.
//

#include "CurvePath.h"
#include <threepp/extras/curve/Line.h>

namespace three {
namespace extras {

using namespace curve;
using namespace math;
using namespace std;

void CurvePath::closePath()
{
  if(_curves.size() < 2) return;

  // Add a line curve if start and end of lines are not connected
  Vector2 startPoint = _curves.front()->getPoint( 0 );
  Vector2 endPoint = _curves.back()->getPoint( 1 );

  if ( startPoint != endPoint ) {

    _curves.push_back( Line::make( endPoint, startPoint ) );
  }
}

Vector2 CurvePath::getPoint( float t )
{
  float d = t * getLength();
  const auto &curveLengths = getCurveLengths();

  // To think about boundaries points.
  for( unsigned i=0; i < curveLengths.size(); i++) {

    if ( curveLengths[ i ] >= d ) {

      float diff = curveLengths[ i ] - d;
      Curve::Ptr &curve = _curves[ i ];

      float segmentLength = curve->getLength();
      float u = segmentLength == 0 ? 0 : 1 - diff / segmentLength;

      return curve->getPointAt( u );
    }
  }

  return Vector2();

  // loop where sum != 0, sum > d , sum+1 <d
}

void CurvePath::getPoints(unsigned divisions, vector<Vector2> &points)
{
   for (unsigned i = 0, l = _curves.size(); i < l; i ++ ) {

     const Curve::Ptr &curve = _curves[ i ];
     unsigned resolution = curve->pathResolution(divisions);

     vector<Vector2> pts;
     curve->getPoints( resolution, pts );

     Vector2 last;
     for (const auto &point : pts) {

       if ( last == point) continue; // ensures no consecutive points are duplicates

       points.push_back( point );
       last = point;
     }
   }

   if ( _autoClose && !points.empty() && points.back() != points.front() ) {

     points.push_back( points.front() );
   }
}

void CurvePath::getSpacedPoints(unsigned divisions, vector<Vector2> &points)
{
   for (auto i = 0; i <= divisions; i ++ ) {

     points.push_back( getPoint( (float)i / divisions ) );
   }

   if ( _autoClose ) {

     points.push_back( points[ 0 ] );
   }
}

}
}