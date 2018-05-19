//
// Created by byter on 06.09.17.
//

#include <vector>
#include <threepp/objects/Points.h>
#include <threepp/objects/Line.h>
#include <threepp/objects/Mesh.h>

#include "BufferGeometry.h"
#include "DirectGeometry.h"
#include "impl/raycast.h"

namespace three {

using namespace math;
using namespace impl;

size_t Geometry::id_count = 0;


math::Vector3 BufferGeometry::centroid(const Face3 &face) const
{
  math::Vector3 vA = math::Vector3::fromBufferAttribute(*_position, face.a);
  math::Vector3 vB = math::Vector3::fromBufferAttribute(*_position, face.b);
  math::Vector3 vC = math::Vector3::fromBufferAttribute(*_position, face.c);

  return (vA + vB + vC) / 3.0f;
}

void BufferGeometry::raycast(Mesh &mesh,
                             const Raycaster &raycaster,
                             const std::vector<math::Ray> &rays,
                             std::vector<Intersection> &intersects)
{
  if (_index != nullptr) {

    // indexed buffer geometry
    for (size_t i = 0, l = _index->size(); i < l; i += 3) {

      uint32_t a = _index->get_x(i);
      uint32_t b = _index->get_x(i + 1);
      uint32_t c = _index->get_x(i + 2);

      Intersection intersection;
      for(const auto &ray : rays) {
        if(checkBufferGeometryIntersection(mesh, raycaster, ray, _position, _uv, a, b, c, intersection)) {
          intersection.faceIndex = (unsigned)std::floor(i / 3);
          intersects.push_back(intersection);
        }
      }
    }
  }
  else {
    // non-indexed buffer geometry
    for (unsigned i = 0, l = (unsigned) _position->itemCount(); i < l; i += 3) {

      unsigned a = i;
      unsigned b = i + 1;
      unsigned c = i + 2;

      Intersection intersection;
      for(const auto &ray : rays) {
        if (checkBufferGeometryIntersection(mesh, raycaster, ray, _position, _uv, a, b, c, intersection)) {
          intersection.index = a;
          intersects.push_back(intersection);
        }
      }
    }
  }
}

void BufferGeometry::raycast(Line &line,
                             const Raycaster &raycaster,
                             const std::vector<math::Ray> &rays,
                             std::vector<Intersection> &intersects)
{
  float precisionSq = raycaster.linePrecision() * raycaster.linePrecision();
  unsigned step = line.steps();

  Vector3 interSegment;
  Vector3 interRay;

  if (_index != nullptr) {

    for (size_t i = 0, l = _index->size() - 1; i < l; i += step ) {

      uint32_t a = (*_index)[ i ];
      uint32_t b = (*_index)[ i + 1 ];

      Vector3 vStart = Vector3::fromArray(_position->data_t(), a * 3 );
      Vector3 vEnd = Vector3::fromArray(_position->data_t(), b * 3 );

      for(const auto &ray : rays) {
        float distSq = ray.distanceSqToSegment( vStart, vEnd, &interRay, &interSegment );

        if ( distSq > precisionSq ) continue;

        interRay.apply( line.matrixWorld() ); //Move back to world space for distance calculation

        float distance = raycaster.origin().distanceTo( interRay );

        if ( distance < raycaster.near() || distance > raycaster.far() ) continue;

        intersects.emplace_back();
        Intersection &intersection = intersects.back();

        intersection.distance = distance;
        // What do we want? intersection point on the ray or on the segment??
        // point: raycaster.ray.at( distance ),
        intersection.point = interSegment.apply(line.matrixWorld());
        intersection.index = i;
        intersection.object = &line;
      }
    }
  } else {

    for (size_t i = 0, l = _position->size() / 3 - 1; i < l; i += step ) {

      Vector3 vStart = Vector3::fromArray(_position->data_t(), 3 * i );
      Vector3 vEnd = Vector3::fromArray(_position->data_t(), 3 * i + 3 );

      for(const auto &ray : rays) {
        float distSq = ray.distanceSqToSegment(vStart, vEnd, &interRay, &interSegment);

        if (distSq > precisionSq) continue;

        interRay.apply(line.matrixWorld()); //Move back to world space for distance calculation

        float distance = raycaster.origin().distanceTo(interRay);

        if (distance < raycaster.near() || distance > raycaster.far()) continue;

        intersects.emplace_back();
        Intersection &intersection = intersects.back();

        intersection.distance = distance;
        // What do we want? intersection point on the ray or on the segment??
        // point: raycaster.ray.at( distance ),
        intersection.point = interSegment.apply(line.matrixWorld());
        intersection.index = i;
        intersection.object = &line;
      }
    }
  }
}

BufferGeometry::BufferGeometry(Object3D::Ptr object, LinearGeometry &geometry)
   : Geometry(geometry::Typer(this))
{
  if(Line *line = object->typer) {
    setFromLinearGeometry(geometry);
  }
  else if(Points *points = object->typer) {
    setFromLinearGeometry(geometry);
  }
  else if(Mesh *mesh = object->typer) {
    setFromMeshGeometry(geometry);
  }
}

void BufferGeometry::setFromLinearGeometry(const LinearGeometry &geometry)
{
  _position = attribute::copied<float, Vertex>(geometry._vertices);
  _color = attribute::copied<float, Color>(geometry._colors);

  _boundingSphere = geometry._boundingSphere;
  _boundingBox = geometry._boundingBox;

  if (geometry._lineDistances.size() == geometry._vertices.size()) {

    _lineDistances = attribute::copied<float>(geometry._lineDistances);
  }
}

void BufferGeometry::setFromMeshGeometry(LinearGeometry &geometry)
{
  geometry._directGeometry = DirectGeometry::make(geometry);

  setFromDirectGeometry( geometry._directGeometry );
}

void BufferGeometry::setFromDirectGeometry(DirectGeometry::Ptr geometry)
{
  _position = attribute::copied<float, Vertex>( geometry->vertices);

  if (!geometry->normals.empty())
    _normal = attribute::copied<float, Vertex>(geometry->normals);

  if (!geometry->colors.empty())
    _color = attribute::copied<float, Color>(geometry->colors);

  if (!geometry->uvs.empty())
    _uv = attribute::copied<float, UV>(geometry->uvs);

  if (!geometry->uv2s.empty())
    _uv2 = attribute::copied<float, UV>(geometry->uv2s);

  if (!geometry->indices.empty())
    _index = attribute::copied<uint32_t, Index>(geometry->indices);

  // groups
  _groups = geometry->groups;

  // morphs
  for(const auto &morphTargets : geometry->morphTargetsPosition)
    _morphAttributes_position.push_back(attribute::copied<float, Vertex>(morphTargets));

  for(const auto &morphTargets : geometry->morphTargetsNormal)
    _morphAttributes_normal.push_back(attribute::copied<float, Vertex>(morphTargets));

  // skinning
  if (!geometry->skinIndices.empty())
    _skinIndices = attribute::copied<float, math::Vector4>(geometry->skinIndices);

  if (!geometry->skinWeights.empty())
    _skinWeight = attribute::copied<float, math::Vector4>(geometry->skinWeights);

  _boundingSphere = geometry->boundingSphere();

  _boundingBox = geometry->boundingBox();
}

BufferGeometry &BufferGeometry::update(Object3D::Ptr object, LinearGeometry *geometry)
{
  Mesh *mesh = object->typer;
  if ( mesh ) {

    DirectGeometry::Ptr direct = geometry->_directGeometry;

    if(geometry->_elementsNeedUpdate) {

      direct = nullptr;
      geometry->_elementsNeedUpdate = false;
    }

    if (!direct) {

      setFromMeshGeometry(*geometry);
      return *this;
    }

    direct->verticesNeedUpdate = geometry->_verticesNeedUpdate;
    direct->normalsNeedUpdate = geometry->_normalsNeedUpdate;
    direct->colorsNeedUpdate = geometry->_colorsNeedUpdate;
    direct->uvsNeedUpdate = geometry->_uvsNeedUpdate;
    direct->groupsNeedUpdate = geometry->_groupsNeedUpdate;

    geometry->_verticesNeedUpdate = false;
    geometry->_normalsNeedUpdate = false;
    geometry->_colorsNeedUpdate = false;
    geometry->_uvsNeedUpdate = false;
    geometry->_groupsNeedUpdate = false;

    if ( direct->verticesNeedUpdate ) {

      if ( _position ) {

        _position = attribute::copied<float, Vertex>(direct->vertices);
        _position->needsUpdate();
      }

      direct->verticesNeedUpdate = false;
    }

    if (direct->normalsNeedUpdate) {

      if (_normal) {

        _normal = attribute::copied<float, Vertex>(direct->normals);
        _normal->needsUpdate();
      }

      direct->normalsNeedUpdate = false;
    }

    if (direct->colorsNeedUpdate ) {

      if (_color) {

        _color = attribute::copied<float, Color>(direct->colors);
        _color->needsUpdate();
      }

      direct->colorsNeedUpdate = false;
    }
    if (direct->uvsNeedUpdate) {

      if (_uv) {

        _uv = attribute::copied<float, UV>(direct->uvs);
        _uv->needsUpdate();
      }

      direct->uvsNeedUpdate = false;
    }
    if ( direct->groupsNeedUpdate && direct) {

      direct->computeGroups( *geometry );
      _groups = direct->groups;

      direct->groupsNeedUpdate = false;
    }
  }
  else {
    if ( geometry->_verticesNeedUpdate ) {

      if ( _position ) {

        _position = attribute::copied<float, Vertex>(geometry->_vertices);
        _position->needsUpdate();
      }

      geometry->_verticesNeedUpdate = false;
    }

    if (geometry->_normalsNeedUpdate) {

      if (_normal) {

        _normal = attribute::copied<float, Vertex>(geometry->_normals);
        _normal->needsUpdate();
      }

      geometry->_normalsNeedUpdate = false;
    }

    if (geometry->_colorsNeedUpdate ) {

      if (_color) {

        _color = attribute::copied<float, Color>(geometry->_colors);
        _color->needsUpdate();
      }

      geometry->_colorsNeedUpdate = false;
    }
    if (geometry->_lineDistancesNeedUpdate ) {

      if (_lineDistances) {

        _lineDistances = attribute::copied<float>(geometry->_lineDistances);
        _lineDistances->needsUpdate();
      }

      geometry->_lineDistancesNeedUpdate = false;
    }
  }
  return *this;
}

void BufferGeometry::normalizeNormals()
{
  for (unsigned i = 0, il = _normal->itemCount(); i < il; i ++ ) {
    _normal->item_at<Vector3>(i).normalize();
  }
}

void BufferGeometry::computeVertexNormals()
{
  if(_position) {

    if (_normal)
      // reset existing normals to zero
      _normal->clear();
    else
      _normal = attribute::prealloc<float, Vector3>(_position->itemCount());


    // indexed elements
    if ( _index ) {
      //var indices = index.array;

      if (_groups.empty()) {

        addGroup( 0, _index->size());
      }

      for (unsigned j = 0, jl = _groups.size(); j < jl; ++ j ) {

        Group &group = _groups[ j ];

        for (unsigned i = group.start, il = group.start + group.count; i < il; i += 3 ) {

          unsigned vA = _index->at(i) * 3;
          unsigned vB = _index->at(i+1) * 3;
          unsigned vC = _index->at(i+2) * 3;

          Vector3 pA = Vector3::fromBufferAttribute(*_position, vA);
          Vector3 pB = Vector3::fromBufferAttribute(*_position, vB);
          Vector3 pC = Vector3::fromBufferAttribute(*_position, vC);

          Vector3 cb = pC - pB;
          Vector3 ab = pA - pB;
          cb.cross( ab );

          _normal->at(vA) += cb.x();
          _normal->at(vA + 1) += cb.y();
          _normal->at(vA + 2) += cb.z();

          _normal->at(vB) += cb.x();
          _normal->at(vB + 1) += cb.y();
          _normal->at(vB + 2) += cb.z();

          _normal->at(vC) += cb.x();
          _normal->at(vC + 1) += cb.y();
          _normal->at(vC + 2) += cb.z();
        }
      }

    } else {
      // non-indexed elements (unconnected triangle soup)
      for (unsigned i = 0, il = _position->itemCount(); i < il; i += 3 ) {

        Vector3 pA = Vector3::fromBufferAttribute(*_position, i);
        Vector3 pB = Vector3::fromBufferAttribute(*_position, i+1);
        Vector3 pC = Vector3::fromBufferAttribute(*_position, i+2);

        Vector3 cb = pC - pB;
        Vector3 ab = pA - pB;
        cb.cross( ab );

        for(unsigned ix=0; ix < 3; ix++) {
          _normal->set_x(ix, cb.x());
          _normal->set_y(ix, cb.y());
          _normal->set_z(ix, cb.z());
        }
      }
    }

    normalizeNormals();
    _normal->needsUpdate();
  }
}

}
