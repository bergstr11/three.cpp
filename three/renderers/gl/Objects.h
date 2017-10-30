//
// Created by byter on 20.09.17.
//

#ifndef THREE_QT_OBJECTS_H
#define THREE_QT_OBJECTS_H

#include <vector>
#include <core/Object3D.h>
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
    if (_updateList.find(buffergeometry->id) == _updateList.end() || _updateList[ buffergeometry->id] != frame ) {

      if (std::dynamic_pointer_cast<BufferGeometry::Ptr>(geometry) == nullptr) {
        buffergeometry->update( object );
      }
      _geometries.update( buffergeometry );

      _updateList[ buffergeometry->id ] = frame;
    }

    return buffergeometry;
  }
};

}
}
#endif //THREE_QT_OBJECTS_H
