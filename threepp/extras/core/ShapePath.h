#pragma once

/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 * minimal class for proxing functions to Path. Replaces old "extractSubpaths()"
 **/

#include <threepp/core/Color.h>
#include "Path.h"
#include "Shape.h"
#include "../ShapeUtils.h"

namespace three {
namespace extras {

class DLX ShapePath
{
	Color _color;

	std::vector<Path::Ptr> subPaths;
	Path::Ptr currentPath;

  ShapePath &moveTo( float x, float y ) {

		currentPath = Shape::make();
		subPaths.push_back( currentPath );
		currentPath->moveTo( x, y );
		return *this;
	}

  ShapePath &lineTo( float x, float y ) {

		currentPath->lineTo( x, y );
    return *this;
	}

  ShapePath &quadraticCurveTo( float aCPx, float aCPy, float aX, float aY ) {

		currentPath->quadraticCurveTo( aCPx, aCPy, aX, aY );
    return *this;
	}

  ShapePath &bezierCurveTo( float aCP1x, float aCP1y, float aCP2x, float aCP2y, float aX, float aY ) {

		currentPath->bezierCurveTo( aCP1x, aCP1y, aCP2x, aCP2y, aX, aY );
    return *this;
	}

  ShapePath &splineThru( const std::vector<math::Vector2> &pts ) {

		currentPath->splineThru( pts );
    return *this;
	}

	std::vector<Shape::Ptr> toShapes( bool isCCW, bool noHoles );
};

}
}
