//
// Created by byter on 5/27/18.
//

#ifndef THREE_PP_SHAPEUTILS_H
#define THREE_PP_SHAPEUTILS_H

#include <vector>
#include <threepp/math/Vector2.h>

namespace three {
namespace extras {

namespace shapeutils
{

// calculate area of the contour polygon

float area(const std::vector<math::Vector2> &contour)
{
   auto n = contour.size();
   float a = 0.0f;

   for (auto p = n - 1, q = 0u; q < n; p = q++ ) {

     a += contour[ p ].x() * contour[ q ].y() - contour[ q ].x() * contour[ p ].y();
   }

   return a * 0.5f;
}

bool isClockWise(const std::vector<math::Vector2> &pts)
{
   return area( pts ) < 0;
}

void triangulateShape(const std::vector<math::Vector2> &contour, holes )
{
var vertices = []; // flat array of vertices like [ x0,y0, x1,y1, x2,y2, ... ]
var holeIndices = []; // array of hole indices
var faces = []; // final array of vertex indices like [ [ a,b,d ], [ b,c,d ] ]

removeDupEndPts( contour );
addContour( vertices, contour );

//

var holeIndex = contour.length;

holes.forEach( removeDupEndPts );

for ( var i = 0; i < holes.length; i ++ ) {

holeIndices.push( holeIndex );
holeIndex += holes[ i ].length;
addContour( vertices, holes[ i ] );

}

//

var triangles = Earcut.triangulate( vertices, holeIndices );

//

for ( var i = 0; i < triangles.length; i += 3 ) {

faces.push( triangles.slice( i, i + 3 ) );

}

return faces;

}

function removeDupEndPts( points ) {

  var l = points.length;

  if ( l > 2 && points[ l - 1 ].equals( points[ 0 ] ) ) {

    points.pop();

  }

}

void addContour( vertices, contour )
{
  for ( var i = 0; i < contour.length; i ++ ) {

    vertices.push( contour[ i ].x );
    vertices.push( contour[ i ].y );

  }
}

}

}
}
#endif //THREE_PP_SHAPEUTILS_H
