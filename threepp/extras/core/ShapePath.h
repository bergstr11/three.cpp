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

struct DLX ShapePath
{
	std::vector<Path::Ptr> subPaths;
  Path::Ptr currentPath;
  Color color;

  ShapePath &moveTo( float x, float y ) {

		currentPath = Shape::make();
		subPaths.push_back( currentPath );
		currentPath->moveTo( x, y );
		return *this;
	}

  ShapePath &lineTo( float x, float y, bool amend=false ) {

		currentPath->lineTo( x, y );
		if(amend) {
      for(auto it = subPaths.begin(); it != subPaths.end() && *it != currentPath; it++) {
        if((*it)->curves().empty()) (*it)->lineTo(x, y);
      }
		}
    return *this;
	}

  ShapePath &quadraticCurveTo( float aCPx, float aCPy, float aX, float aY, bool amend=false ) {

		currentPath->quadraticCurveTo( aCPx, aCPy, aX, aY );
		if(amend) {
      for(auto it = subPaths.begin(); it != subPaths.end() && *it != currentPath; it++) {
        if((*it)->curves().empty()) (*it)->quadraticCurveTo( aCPx, aCPy, aX, aY );
      }
		}
    return *this;
	}

  ShapePath &bezierCurveTo( float aCP1x, float aCP1y, float aCP2x, float aCP2y, float aX, float aY, bool amend=false )
  {
		currentPath->bezierCurveTo( aCP1x, aCP1y, aCP2x, aCP2y, aX, aY );
		if(amend) {
      for(auto it = subPaths.begin(); it != subPaths.end() && *it != currentPath; it++) {
        if((*it)->curves().empty()) (*it)->bezierCurveTo( aCP1x, aCP1y, aCP2x, aCP2y, aX, aY );
      }
		}
    return *this;
	}

  ShapePath &splineThru( const std::vector<math::Vector2> &pts, bool amend=false ) {

		currentPath->splineThru( pts );
		if(amend) {
      for(auto it = subPaths.begin(); it != subPaths.end() && *it != currentPath; it++) {
        if((*it)->curves().empty()) (*it)->splineThru( pts );
      }
		}
    return *this;
	}

	void close(bool amend=false)
  {
    auto func = [](Path::Ptr &path) {
      path->setAutoClose(true);
      if ( path->curves().size() > 0 ) {
        // Reset point to beginning of Path
        math::Vector2 point = path->curve(0)->start();
        path->moveTo(point.x(), point.y());
      }
    };
    func(currentPath);
    if(amend) {
      for(auto it = subPaths.begin(); it != subPaths.end() && *it != currentPath; it++) {
        if(!(*it)->autoClose()) func(*it);
      }
    }
  }

	std::vector<Shape::Ptr> toShapes( bool isCCW=false, bool noHoles=false ) const;
};

}
}
