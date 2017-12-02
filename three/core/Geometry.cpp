//
// Created by byter on 06.09.17.
//

#include <vector>
#include <objects/Mesh.h>
#include <objects/Line.h>
#include <geometry/Box.h>
#include <geometry/Plane.h>

#include "DirectGeometry.h"
#include "BufferGeometry.h"
#include "impl/raycast.h"

namespace three {

using namespace math;
using namespace impl;

size_t Geometry::id_count = 0;
size_t BufferGeometry::MaxIndex = 65535;

void StaticGeometry::raycast(const Mesh &mesh,
                             const Raycaster &raycaster,
                             const math::Ray &ray,
                             math::Vector3 &intersectionPoint,
                             math::Vector3 &intersectionPointWorld,
                             std::vector<Intersection> &intersects)
{
  std::vector<UV_Array> &faceVertexUvs = _faceVertexUvs[0];

  for (size_t f=0, fl=_faces.size(); f < fl; f++) {

    const Face3 &face = _faces[f];
    Material::Ptr faceMaterial = mesh.material(face.materialIndex);

    if (!faceMaterial) continue;

    Vector3 &fvA = _vertices[face.a];
    Vector3 &fvB = _vertices[face.b];
    Vector3 &fvC = _vertices[face.c];

    if (faceMaterial->morphTargets) {

      math::Vector3 vA(0, 0, 0);
      math::Vector3 vB(0, 0, 0);
      math::Vector3 vC(0, 0, 0);

      for (size_t t = 0, tl = _morphTargets.size(); t < tl; t++) {

        float influence = mesh.morphTargetInfluence(t);

        if (influence == 0) continue;

        const std::vector<math::Vector3> &targets = _morphTargets[t].vertices;

        vA += (targets[face.a] - fvA) * influence;
        vB += (targets[face.b] - fvB) * influence;
        vC += (targets[face.c] - fvC) * influence;
      }

      fvA += vA;
      fvB += vB;
      fvC += vC;
    }

    intersects.emplace_back();
    Intersection &intersection = intersects.back();
    bool hasIntersect = checkIntersection(mesh, faceMaterial, raycaster, ray, fvA, fvB, fvC, intersectionPoint, intersection);

    if (hasIntersect) {

      if (faceVertexUvs.size() > f) {

        UV_Array &uvs_f = faceVertexUvs[f];
        Vector2 uvA(uvs_f[0]);
        Vector2 uvB(uvs_f[1]);
        Vector2 uvC(uvs_f[2]);

        intersection.uv = uvIntersection(intersectionPoint, fvA, fvB, fvC, uvA, uvB, uvC);
      }

      intersection.face = face;
      intersection.faceIndex = (unsigned)f;
    }
    else intersects.pop_back();
  }
}

void StaticGeometry::raycast(const Line &line,
                             const Raycaster &raycaster,
                             const math::Ray &ray,
                             std::vector<Intersection> &intersects)
{
  Vector3 interSegment;
  Vector3 interRay;
  unsigned step = line.steps();

  float precisionSq = raycaster.linePrecision() * raycaster.linePrecision();

  for (size_t i = 0; i < _vertices.size() - 1; i += step ) {

    float distSq = ray.distanceSqToSegment(_vertices[i], _vertices[i + 1], &interRay, &interSegment);

    if (distSq > precisionSq) continue;

    interRay.apply(line.matrixWorld()); //Move back to world space for distance calculation

    float distance = raycaster.ray().origin().distanceTo(interRay);

    if (distance < raycaster.near() || distance > raycaster.far()) continue;

    intersects.emplace_back();
    Intersection &intersect = intersects.back();
    intersect.distance = distance;
    // What do we want? intersection point on the ray or on the segment??
    // point: raycaster.ray.at( distance ),
    intersect.point = interSegment.apply(line.matrixWorld());
    intersect.index = i;
    intersect.object = &line;
  }
}

void BufferGeometry::raycast(const Mesh &mesh,
                             const Raycaster &raycaster,
                             const math::Ray &ray,
                             math::Vector3 &intersectionPoint,
                             math::Vector3 &intersectionPointWorld,
                             std::vector<Intersection> &intersects)
{
  if (_index != nullptr) {

    // indexed buffer geometry
    for (size_t i = 0, l = _index->count(); i < l; i += 3) {

      uint32_t a = _index->get_x(i);
      uint32_t b = _index->get_x(i + 1);
      uint32_t c = _index->get_x(i + 2);

      intersects.emplace_back();
      Intersection &intersection = intersects.back();

      bool isIntersect = checkBufferGeometryIntersection(mesh, raycaster, ray, _position, _uv, a, b, c, intersectionPoint, intersection);
      if (isIntersect) {
        intersection.faceIndex = (unsigned)std::floor(i / 3); // triangle number in indices buffer semantics
      }
      else intersects.pop_back();
    }
  }
  else {

    // non-indexed buffer geometry
    for (unsigned i = 0, l = (unsigned)_position->count(); i < l; i += 3) {

      unsigned a = i;
      unsigned b = i + 1;
      unsigned c = i + 2;

      intersects.emplace_back();
      Intersection &intersection = intersects.back();

      bool isIntersect = checkBufferGeometryIntersection(mesh, raycaster, ray, _position, _uv, a, b, c, intersectionPoint, intersection);

      if (isIntersect) {
        intersection.index = a; // triangle number in positions buffer semantics
      }
      else intersects.pop_back();
    }
  }
}

void BufferGeometry::raycast(const Line &line,
                             const Raycaster &raycaster, const math::Ray &ray,
                             std::vector<Intersection> &intersects)
{
  float precisionSq = raycaster.linePrecision() * raycaster.linePrecision();
  unsigned step = line.steps();

  Vector3 interSegment;
  Vector3 interRay;

  if (_index != nullptr) {

    for (size_t i = 0, l = _index->array().size() - 1; i < l; i += step ) {

      uint32_t a = (*_index)[ i ];
      uint32_t b = (*_index)[ i + 1 ];

      Vector3 vStart = Vector3::fromArray(_position->array(), a * 3 );
      Vector3 vEnd = Vector3::fromArray(_position->array(), b * 3 );

      float distSq = ray.distanceSqToSegment( vStart, vEnd, &interRay, &interSegment );

      if ( distSq > precisionSq ) continue;

      interRay.apply( line.matrixWorld() ); //Move back to world space for distance calculation

      float distance = raycaster.ray().origin().distanceTo( interRay );

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
  } else {

    for (size_t i = 0, l = _position->count() / 3 - 1; i < l; i += step ) {

      Vector3 vStart = Vector3::fromArray(_position->array(), 3 * i );
      Vector3 vEnd = Vector3::fromArray(_position->array(), 3 * i + 3 );

      Vector3 interSegment;
      Vector3 interRay;
      float distSq = ray.distanceSqToSegment( vStart, vEnd, &interRay, &interSegment );

      if ( distSq > precisionSq ) continue;

      interRay.apply( line.matrixWorld() ); //Move back to world space for distance calculation

      float distance = raycaster.ray().origin().distanceTo( interRay );

      if ( distance < raycaster.near() || distance > raycaster.far() ) continue;

      intersects.emplace_back();
      Intersection &intersection = intersects.back();
      intersection.distance = distance;
                          // What do we want? intersection point on the ray or on the segment??
                          // point: raycaster.ray.at( distance ),
      intersection.point = interSegment.apply( line.matrixWorld() ),
         intersection.index = i,
         intersection.object = &line;
    }
  }
}

BufferGeometry::BufferGeometry(Object3D::Ptr object)
{
  object->geometry()->toBufferGeometry(*this);
}

void three::geometry::Plane::toBufferGeometry(BufferGeometry &geometry)
{
  StaticGeometry::toBufferGeometry(geometry);
}

void three::geometry::Box::toBufferGeometry(BufferGeometry &geometry)
{
  StaticGeometry::toBufferGeometry(geometry);
}

void three::StaticGeometry::toBufferGeometry(BufferGeometry &geometry)
{
  BufferAttributeT<float>::Ptr positions = BufferAttributeT<float>::make(_vertices);
  BufferAttributeT<float>::Ptr colors = BufferAttributeT<float>::make(_colors);

  geometry.setPosition(positions);
  geometry.setColor(colors);

  geometry.boundingSphere() = _boundingSphere;
  geometry.boundingBox() = _boundingBox;

  if (_lineDistances.size() == _vertices.size()) {

    BufferAttributeT<float>::Ptr lineDistances = BufferAttributeT<float>::make(_lineDistances);

    geometry.setLineDistances(lineDistances);
  }
}

BufferGeometry &BufferGeometry::update(Object3D::Ptr object, StaticGeometry::Ptr geometry)
{
  Mesh::Ptr mesh = std::dynamic_pointer_cast<Mesh>(object);
  if ( mesh ) {

    DirectGeometry::Ptr direct = geometry->_directGeometry;

    if(geometry->_elementsNeedUpdate) {

      direct = nullptr;
      geometry->_elementsNeedUpdate = false;
    }

    if (!direct) {

      geometry->toBufferGeometry(*this);
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

        _position = BufferAttributeT<float>::make(direct->vertices);
        _position->needsUpdate();
      }

      direct->verticesNeedUpdate = false;
    }

    if (direct->normalsNeedUpdate) {

      if (_normal) {

        _normal = BufferAttributeT<float>::make(direct->normals);
        _normal->needsUpdate();
      }

      direct->normalsNeedUpdate = false;
    }

    if (direct->colorsNeedUpdate ) {

      if (_color) {

        _color = BufferAttributeT<float>::make(direct->colors);
        _color->needsUpdate();
      }

      direct->colorsNeedUpdate = false;
    }
    if (direct->uvsNeedUpdate) {

      if (_uv) {

        _uv = BufferAttributeT<float>::make(direct->uvs);
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

        _position = BufferAttributeT<float>::make(geometry->_vertices);
        _position->needsUpdate();
      }

      geometry->_verticesNeedUpdate = false;
    }

    if (geometry->_normalsNeedUpdate) {

      if (_normal) {

        _normal = BufferAttributeT<float>::make(geometry->_normals);
        _normal->needsUpdate();
      }

      geometry->_normalsNeedUpdate = false;
    }

    if (geometry->_colorsNeedUpdate ) {

      if (_color) {

        _color = BufferAttributeT<float>::make(geometry->_colors);
        _color->needsUpdate();
      }

      geometry->_colorsNeedUpdate = false;
    }
    if (geometry->_lineDistancesNeedUpdate ) {

      if (_lineDistances) {

        _lineDistances = BufferAttributeT<float>::make(geometry->_lineDistances);
        _lineDistances->needsUpdate();
      }

      geometry->_lineDistancesNeedUpdate = false;
    }
  }
  return *this;
}

}
