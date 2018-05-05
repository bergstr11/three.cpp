//
// Created by byter on 20.09.17.
//

#ifndef THREEPP_OBJECTS_H
#define THREEPP_OBJECTS_H

#include <vector>
#include <threepp/core/Object3D.h>
#include "Helpers.h"
#include "Geometries.h"

namespace three {
namespace gl {

class Objects
{
  std::unordered_map<unsigned, unsigned> _updateList;

  Geometries &_geometries;
  RenderInfo &_infoRender;

public:
  Objects(Geometries &geometries, RenderInfo &infoRender)
     : _geometries(geometries), _infoRender(infoRender)
  {}

  BufferGeometry::Ptr update(Object3D::Ptr object)
  {
    unsigned frame = _infoRender.frame;

    Geometry::Ptr geometry = object->geometry();
    BufferGeometry::Ptr buffergeometry = _geometries.get( object, geometry );

    // Update once per frame
    if (_updateList.count(buffergeometry->id) == 0 || _updateList[ buffergeometry->id] != frame ) {

      LinearGeometry *linearGeom = geometry->typer;
      if (linearGeom) {
        buffergeometry->update( object, linearGeom );
      }
      _geometries.update( buffergeometry );

      _updateList[ buffergeometry->id ] = frame;
    }

    return buffergeometry;
  }
};

}
}
#endif //THREEPP_OBJECTS_H
