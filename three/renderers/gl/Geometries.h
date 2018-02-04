//
// Created by byter on 20.09.17.
//

#ifndef THREE_QT_GEOMETRIES_H
#define THREE_QT_GEOMETRIES_H

#include <unordered_map>

#include <three/core/Object3D.h>
#include <three/core/Geometry.h>
#include <three/core/BufferGeometry.h>
#include "Attributes.h"


namespace three {
namespace gl {

class Geometries
{
  struct GeometryInfo {
    BufferGeometry::Ptr geometry;
    BufferGeometry::OnDispose::ConnectionId connectionId;
  };
  std::unordered_map<unsigned, GeometryInfo> geometries;
  std::unordered_map<unsigned, BufferAttributeT<uint32_t>::Ptr> wireframeAttributes;

  unsigned geometryCount = 0;

  Attributes &_attributes;

  void onGeometryDispose(Geometry *geometry)
  {
    GeometryInfo &gi = geometries[ geometry->id ];
    BufferGeometry::Ptr buffergeometry = gi.geometry;

    if (buffergeometry->index()) {
      _attributes.remove( *buffergeometry->index() );
    }

    if(buffergeometry->position()) _attributes.remove(*buffergeometry->position());
    if(buffergeometry->normal()) _attributes.remove(*buffergeometry->normal());
    if(buffergeometry->color()) _attributes.remove(*buffergeometry->color());
    if(buffergeometry->uv()) _attributes.remove(*buffergeometry->uv());
    if(buffergeometry->uv2()) _attributes.remove(*buffergeometry->uv2());

    geometry->onDispose.disconnect(gi.connectionId);

    geometries.erase(geometry->id);

    // TODO Remove duplicate code
    if (wireframeAttributes.find(geometry->id) != wireframeAttributes.end()) {
      auto &attribute = wireframeAttributes[geometry->id];
      _attributes.remove( *attribute );
      wireframeAttributes.erase(geometry->id);
    }

    if (wireframeAttributes.find(buffergeometry->id) != wireframeAttributes.end()) {
      auto &attribute = wireframeAttributes[buffergeometry->id];
      _attributes.remove( *attribute );
      wireframeAttributes.erase(buffergeometry->id);
    }

    //infoMemory.geometries --;
    geometryCount--;
  }

public:
  Geometries(Attributes &attributes) : _attributes(attributes) {}

  BufferGeometry::Ptr get(Object3D::Ptr object, Geometry::Ptr geometry)
  {
    GeometryInfo &gi = geometries[ geometry->id ];

    if (gi.geometry) return gi.geometry;

    geometry->onDispose.connect(*this, &Geometries::onGeometryDispose);

    BufferGeometry::Ptr buffergeometry = std::dynamic_pointer_cast<BufferGeometry>(geometry);
    if (!buffergeometry) {
      LinearGeometry::Ptr lineargeometry = std::dynamic_pointer_cast<LinearGeometry>(geometry);
      buffergeometry = BufferGeometry::make(object, lineargeometry);
    }

    gi.geometry = buffergeometry;

    geometryCount++;

    return buffergeometry;
  }

  void update(BufferGeometry::Ptr buffergeometry)
  {
    if (buffergeometry->index()) {
      _attributes.update(*buffergeometry->getIndex(), BufferType::ElementArray);
    }

    if(buffergeometry->position()) _attributes.update(*buffergeometry->position(), BufferType::Array);
    if(buffergeometry->normal()) _attributes.update(*buffergeometry->normal(), BufferType::Array);
    if(buffergeometry->color()) _attributes.update(*buffergeometry->color(), BufferType::Array);
    if(buffergeometry->uv()) _attributes.update(*buffergeometry->uv(), BufferType::Array);
    if(buffergeometry->uv2()) _attributes.update(*buffergeometry->uv2(), BufferType::Array);

    // morph targets

    for (BufferAttributeT<float>::Ptr pos : buffergeometry->morphPositions()) {
      _attributes.update(*pos, BufferType::Array);
    }
    for (BufferAttributeT<float>::Ptr normal : buffergeometry->morphNormals()) {
      _attributes.update(*normal, BufferType::Array);
    }
  }

  BufferAttributeT<uint32_t>::Ptr getWireframeAttribute(const BufferGeometry::Ptr geometry)
  {
    BufferAttributeT<uint32_t>::Ptr attribute = wireframeAttributes[ geometry->id ];

    if ( attribute ) return attribute;

    attribute::prealloc_t<uint32_t> indices;

    if (geometry->index()) {

      auto array = geometry->index()->tdata();

      indices = attribute::prealloc<uint32_t>(geometry->index()->size() * 2, false);
      for (size_t i = 0, l = geometry->index()->size(); i < l; i += 3) {

        auto a = array[ i + 0 ];
        auto b = array[ i + 1 ];
        auto c = array[ i + 2 ];

        indices->next() = a;
        indices->next() = b;
        indices->next() = b;
        indices->next() = c;
        indices->next() = c;
        indices->next() = a;
      }
    }
    else {

      auto array = geometry->position()->tdata();

      indices = attribute::prealloc<uint32_t>(((geometry->position()->size() / 3) - 1) * 2, false);
      for (size_t i = 0, l = (geometry->position()->size() / 3) - 1; i < l; i += 3 ) {

        uint32_t a = i + 0;
        uint32_t b = i + 1;
        uint32_t c = i + 2;

        indices->next() = a;
        indices->next() = b;
        indices->next() = b;
        indices->next() = c;
        indices->next() = c;
        indices->next() = a;
      }
    }

    _attributes.update(*indices, BufferType::ElementArray);

    wireframeAttributes[ geometry->id ] = indices;

    return indices;
  }
};

}
}
#endif //THREE_QT_GEOMETRIES_H
