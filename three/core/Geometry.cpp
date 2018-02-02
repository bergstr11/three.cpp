//
// Created by byter on 06.09.17.
//

#include <vector>
#include <objects/Mesh.h>
#include <objects/Line.h>
#include <objects/Points.h>

#include "DirectGeometry.h"
#include "impl/raycast.h"

namespace three {

using namespace math;
using namespace impl;

size_t Geometry::id_count = 0;
size_t BufferGeometry::MaxIndex = 65535;

void LinearGeometry::raycast(const Mesh &mesh,
                             const Raycaster &raycaster,
                             const math::Ray &ray,
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

    Intersection intersection;
    if (checkIntersection(mesh, faceMaterial, raycaster, ray, fvA, fvB, fvC, intersection)) {

      if (faceVertexUvs.size() > f) {

        UV_Array &uvs_f = faceVertexUvs[f];
        Vector2 uvA(uvs_f[0]);
        Vector2 uvB(uvs_f[1]);
        Vector2 uvC(uvs_f[2]);

        intersection.uv = uvIntersection(intersection.point, fvA, fvB, fvC, uvA, uvB, uvC);
      }

      intersection.face = face;
      intersection.faceIndex = (unsigned)f;
      intersects.push_back(intersection);
    }
  }
}

void LinearGeometry::raycast(const Line &line,
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
                             std::vector<Intersection> &intersects)
{
  if (_index != nullptr) {

    // indexed buffer geometry
    for (size_t i = 0, l = _index->size(); i < l; i += 3) {

      uint32_t a = _index->get_x(i);
      uint32_t b = _index->get_x(i + 1);
      uint32_t c = _index->get_x(i + 2);

      Intersection intersection;
      if(checkBufferGeometryIntersection(mesh, raycaster, ray, _position, _uv, a, b, c, intersection)) {
        intersection.faceIndex = (unsigned)std::floor(i / 3); // triangle number in indices buffer semantics
        intersects.push_back(intersection);
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
      if (checkBufferGeometryIntersection(mesh, raycaster, ray, _position, _uv, a, b, c, intersection)) {
        intersection.index = a; // triangle number in positions buffer semantics
        intersects.push_back(intersection);
      }
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

    for (size_t i = 0, l = _index->size() - 1; i < l; i += step ) {

      uint32_t a = (*_index)[ i ];
      uint32_t b = (*_index)[ i + 1 ];

      Vector3 vStart = Vector3::fromArray(_position->tdata(), a * 3 );
      Vector3 vEnd = Vector3::fromArray(_position->tdata(), b * 3 );

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

    for (size_t i = 0, l = _position->size() / 3 - 1; i < l; i += step ) {

      Vector3 vStart = Vector3::fromArray(_position->tdata(), 3 * i );
      Vector3 vEnd = Vector3::fromArray(_position->tdata(), 3 * i + 3 );

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
  }
}

BufferGeometry::BufferGeometry(Object3D::Ptr object, LinearGeometry::Ptr geometry)
{
  object::Dispatch dispatch;

  dispatch.func<Line>() = [this, geometry](Line &line) {
    setFromLinearGeometry(geometry);
  };
  dispatch.func<LineSegments>() = [this, geometry](LineSegments &lineSegments) {
    setFromLinearGeometry(geometry);
  };
  dispatch.func<Points>() = [this, geometry](Points &points) {
    setFromLinearGeometry(geometry);
  };
  dispatch.func<Mesh>() = [this, geometry](Mesh &mesh) {
    setFromMeshGeometry(geometry);
  };
  object->objectResolver->object::DispatchResolver::getValue(dispatch);
}

void BufferGeometry::setFromLinearGeometry(LinearGeometry::Ptr geometry)
{
  _position = attribute::copied<float, Vertex>(geometry->_vertices);
  _color = attribute::copied<float, Color>(geometry->_colors);

  _boundingSphere = geometry->_boundingSphere;
  _boundingBox = geometry->_boundingBox;

  if (geometry->_lineDistances.size() == geometry->_vertices.size()) {

    _lineDistances = attribute::copied<float>(geometry->_lineDistances);
  }
}

void BufferGeometry::setFromMeshGeometry(LinearGeometry::Ptr geometry)
{
  geometry->_directGeometry = DirectGeometry::make(*geometry);

  setFromDirectGeometry( geometry->_directGeometry );
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

BufferGeometry &BufferGeometry::update(Object3D::Ptr object, LinearGeometry::Ptr geometry)
{
  Mesh::Ptr mesh = std::dynamic_pointer_cast<Mesh>(object);
  if ( mesh ) {

    DirectGeometry::Ptr direct = geometry->_directGeometry;

    if(geometry->_elementsNeedUpdate) {

      direct = nullptr;
      geometry->_elementsNeedUpdate = false;
    }

    if (!direct) {

      setFromMeshGeometry(geometry);
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

}
