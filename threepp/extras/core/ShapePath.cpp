//
// Created by byter on 6/16/18.
//

#include "ShapePath.h"
#include "../ShapeUtils.h"

namespace three {
namespace extras {

using namespace std;
using namespace math;

vector<Shape::Ptr> toShapesNoHoles( std::vector<Path::Ptr> paths )
{
  vector<Shape::Ptr> shapes;

  for (const auto &path : paths) {

    Shape::Ptr tmpShape = Shape::make(path->curves());
    shapes.push_back( tmpShape );
  }

  return shapes;
}

bool isPointInsidePolygon( const Vector2 &inPt, const vector<Vector2> &inPolygon )
{
  auto polyLen = inPolygon.size();

  // inPt on polygon contour => immediate success    or
  // toggling of inside/outside at every single! intersection point of an edge
  //  with the horizontal line through inPt, left of inPt
  //  not counting lowerY endpoints of edges and whole edges on that line
  bool inside = false;
  for ( unsigned p = polyLen - 1, q = 0; q < polyLen; p = q ++ ) {

    const Vector2 &edgeLowPt = inPolygon[ p ];
    const Vector2 &edgeHighPt = inPolygon[ q ];

    float edgeDx = edgeHighPt.x() - edgeLowPt.x();
    float edgeDy = edgeHighPt.y() - edgeLowPt.y();

    if ( std::abs( edgeDy ) > numeric_limits<float>::epsilon()) {

      // not parallel
      if ( edgeDy < 0 ) {

        const Vector2 &edgeLowPt = inPolygon[ q ]; edgeDx = - edgeDx;
        const Vector2 &edgeHighPt = inPolygon[ p ]; edgeDy = - edgeDy;
      }
      if ( ( inPt.y() < edgeLowPt.y() ) || ( inPt.y() > edgeHighPt.y() ) ) 		continue;

      if ( inPt.y() == edgeLowPt.y() ) {

        if ( inPt.x() == edgeLowPt.x() )		return	true;		// inPt is on contour ?
        // continue;				// no intersection or edgeLowPt => doesn't count !!!

      } else {

        float perpEdge = edgeDy * ( inPt.x() - edgeLowPt.x() ) - edgeDx * ( inPt.y() - edgeLowPt.y() );
        if ( perpEdge == 0 ) return	true;		// inPt is on contour ?
        if ( perpEdge < 0 ) continue;
        inside = !inside;		// true intersection left of inPt
      }
    }
    else {

      // parallel or collinear
      if ( inPt.y() != edgeLowPt.y() ) 		continue;			// parallel
      // edge lies on the same horizontal line as inPt
      if ( ( ( edgeHighPt.x() <= inPt.x() ) && ( inPt.x() <= edgeLowPt.x() ) ) ||
           ( ( edgeLowPt.x() <= inPt.x() ) && ( inPt.x() <= edgeHighPt.x() ) ) ) return	true;	// inPt: Point on contour !
      // continue;
    }
  }

  return	inside;
}

struct NewShape
{
  Shape::Ptr s;
  vector<Vector2> p;

  NewShape() = default;
  NewShape(Shape::Ptr s, vector<Vector2> p) :s(s), p(p) {}
};

struct Hole
{
  Path::Ptr h;
  Vector2 p;

  Hole(Path::Ptr h, const Vector2 &p) : h(h), p(p) {}
};

struct Change
{
  unsigned from, to, hole;

  Change(unsigned from, unsigned to, unsigned hole) : from(from), to(to), hole(hole) {}
};

vector<Shape::Ptr> ShapePath::toShapes( bool isCCW, bool noHoles )
{
  if ( subPaths.empty() ) return vector<Shape::Ptr>();

  if ( noHoles )	return	toShapesNoHoles( subPaths );

  if ( subPaths.size() == 1 ) {

    Path::Ptr tmpPath = subPaths[ 0 ];
    return {Shape::make(tmpPath->curves())};
  }

  bool holesFirst = !shapeutils::isClockWise( subPaths[0]->getPoints() );
  holesFirst = isCCW ? ! holesFirst : holesFirst;

  vector<vector<Hole>> betterShapeHoles;
  vector<NewShape> newShapes;
  vector<vector<Hole>> newShapeHoles;

  unsigned mainIdx = 0;

  newShapes.emplace_back(NewShape());;
  newShapeHoles.resize(mainIdx);

  for ( unsigned i = 0, l = subPaths.size(); i < l; i ++ ) {

    Path::Ptr tmpPath = subPaths[ i ];
    vector<Vector2> tmpPoints = tmpPath->getPoints();
    bool solid = shapeutils::isClockWise( tmpPoints );
    solid = isCCW ? ! solid : solid;

    if ( solid ) {

      if (( !holesFirst ) && ( newShapes[ mainIdx ].s ) )	mainIdx ++;

      newShapes.resize(mainIdx);
      newShapes.back() = { Shape::make(tmpPath->curves()), tmpPoints };

      if ( holesFirst )	mainIdx ++;
      newShapeHoles.resize(mainIdx);
    }
    else {

      newShapeHoles[ mainIdx ].emplace_back( tmpPath, tmpPoints[ 0 ] );
    }
  }

  // only Holes? -> probably all Shapes with wrong orientation
  if ( ! newShapes[ 0 ].s )	return	toShapesNoHoles( subPaths );

  if ( newShapes.size() > 1 ) {

    bool ambiguous = false;
    vector<Change> toChange;

    betterShapeHoles.resize(newShapes.size());

    for ( unsigned sIdx = 0, sLen = newShapes.size(); sIdx < sLen; sIdx ++ ) {

      const auto &sho = newShapeHoles[ sIdx ];

      for ( unsigned hIdx = 0; hIdx < sho.size(); hIdx ++ ) {

        const auto &ho = sho[ hIdx ];
        bool hole_unassigned = true;

        for ( unsigned s2Idx = 0; s2Idx < newShapes.size(); s2Idx ++ ) {

          if ( isPointInsidePolygon( ho.p, newShapes[ s2Idx ].p ) ) {

            if ( sIdx != s2Idx )	toChange.emplace_back( sIdx, s2Idx, hIdx );
            if ( hole_unassigned ) {

              hole_unassigned = false;
              betterShapeHoles[ s2Idx ].push_back( ho );

            } else {

              ambiguous = true;
            }
          }
        }
        if ( hole_unassigned ) {

          betterShapeHoles[ sIdx ].push_back( ho );
        }
      }
    }
    if ( !toChange.empty() ) {

      if ( ! ambiguous )	newShapeHoles = betterShapeHoles;
    }
  }

  vector<Shape::Ptr> shapes;
  vector<Hole> tmpHoles;

  for ( unsigned i = 0, il = newShapes.size(); i < il; i ++ ) {

    Shape::Ptr tmpShape = newShapes[ i ].s;
    shapes.push_back( tmpShape );
    tmpHoles = newShapeHoles[ i ];

    for ( unsigned j = 0, jl = tmpHoles.size(); j < jl; j ++ ) {

      tmpShape->_holes.push_back( tmpHoles[ j ].h );
    }
  }

  return shapes;
}

}
}