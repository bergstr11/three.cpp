//
// Created by byter on 08.08.17.
//

#ifndef THREE_QT_STATICGEOMETRY_H
#define THREE_QT_STATICGEOMETRY_H

#include <memory>
#include <vector>
#include <array>
#include <cstdint>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include <math/Vector4.h>
#include <math/Matrix3.h>
#include <helper/UV.h>

#include "BufferGeometry.h"
#include "Geometry.h"
#include "Face3.h"
#include "Color.h"

namespace three {

struct MorphTarget {
  std::string name;
  std::vector<Vertex> vertices;
};
struct MorphNormal {
  std::vector<Vertex> faceNormals;
  std::vector<std::array<Vertex, 3>> vertexNormals;

  bool isEmpty() {return faceNormals.empty();}
};

class StaticGeometry : public Geometry
{
  std::vector<Vertex> _vertices;
  std::vector<Color> _colors; // one-to-one vertex colors, used in ParticleSystem, Line and Ribbon

  std::vector<Face3> _faces;

  using UV_Array = std::array<UV, 3>;
  std::array<std::vector<UV_Array>, 2> _faceVertexUvs;

  std::vector<MorphTarget> _morphTargets;

  std::vector<MorphNormal> _morphNormals;

  std::vector<math::Vector4> _skinWeights;

  std::vector<math::Vector4> _skinIndices;

  std::vector<float> _lineDistances;

  // update flags

  bool _elementsNeedUpdate = false;
  bool _verticesNeedUpdate = false;
  bool _uvsNeedUpdate = false;
  bool _normalsNeedUpdate = false;
  bool _colorsNeedUpdate = false;
  bool _lineDistancesNeedUpdate = false;
  bool _groupsNeedUpdate = false;

  static StaticGeometry &computeFaceNormals(std::vector<Face3> &faces, std::vector<Vertex> vertices)
  {
    for (Face3 & face : faces) {
      Vertex &vA = vertices[ face.a ];
      Vertex &vB = vertices[ face.b ];
      Vertex &vC = vertices[ face.c ];

      Vertex cb = vC - vB;
      Vertex ab = vA - vB;

      face.normal = cb.cross( ab ).normalize();
    }
  }

  static StaticGeometry &computeVertexNormals(std::vector<Face3> &faces, std::vector<Vertex> vertices,
                                        bool areaWeighted=true)
  {
    std::vector<Vertex> verts(vertices.size());

    if ( areaWeighted ) {
      // vertex normals weighted by triangle areas
      // http://www.iquilezles.org/www/articles/normals/normals.htm
      for (const Face3 &face :  faces) {

        const Vertex &vA = vertices[ face.a ];
        const Vertex &vB = vertices[ face.b ];
        const Vertex &vC = vertices[ face.c ];

        Vertex cb = vC - vB;
        Vertex ab = vA - vB;
        cb.cross( ab );

        verts[ face.a ] += cb;
        verts[ face.b ] += cb;
        verts[ face.c ] += cb;
      }
    }
    else {
      computeFaceNormals(faces, vertices);

      for (const Face3 &face : faces) {
        verts[ face.a ] += face.normal;
        verts[ face.b ] += face.normal;
        verts[ face.c ] += face.normal;
      }
    }

    for (Vertex &vertex : verts) {
      vertex.normalize();
    }

    for (Face3 &face : faces) {
      face.vertexNormals[ 0 ] = verts[ face.a ];
      face.vertexNormals[ 1 ] = verts[ face.b ];
      face.vertexNormals[ 2 ] = verts[ face.c ];
    }

    //_normalsNeedUpdate = !_faces.empty();
  }

public:
  using Ptr = std::shared_ptr<StaticGeometry>;

  StaticGeometry() {

  }

  StaticGeometry &apply(const math::Matrix4 &matrix) override
  {
		math::Matrix3 normalMatrix = matrix.normalMatrix();

		for(Vertex &vertex : _vertices) {
			vertex.apply( matrix );
		}

		for(Face3 &face : _faces) {
			face.normal.apply(normalMatrix).normalize();

			for(Vertex &vertexNormal : face.vertexNormals) {
        vertexNormal.apply( normalMatrix ).normalize();
			}
		}

    computeBoundingBox();
		computeBoundingSphere();

		_verticesNeedUpdate = true;
		_normalsNeedUpdate = true;

		return *this;
	}

  Vertex center() const
  {
    return _boundingBox.getCenter().negate();
  }

	StaticGeometry &setCenter()
  {
    computeBoundingBox();

		Vertex offset = _boundingBox.getCenter().negate();

		translate( offset.x(), offset.y(), offset.z() );

		return *this;
	}

	StaticGeometry &normalize()
  {
		computeBoundingSphere();

		const Vertex &center = _boundingSphere.center();
		float radius = _boundingSphere.radius();

		float s = radius == 0 ? 1 : 1.0f / radius;

    apply(math::Matrix4(
			s, 0, 0, - s * center.x(),
			0, s, 0, - s * center.y(),
			0, 0, s, - s * center.z(),
			0, 0, 0, 1
		));
		return *this;
	}

	StaticGeometry &computeFlatVertexNormals()
  {
		computeFaceNormals(_faces, _vertices);

		for (Face3 &face : _faces) {
      face.vertexNormals[ 0 ] = face.normal;
      face.vertexNormals[ 1 ] = face.normal;
      face.vertexNormals[ 2 ] = face.normal;
		}

		_normalsNeedUpdate = !_faces.empty();
	}

	StaticGeometry &computeMorphNormals()
  {
    //copy faces
    std::vector<Face3> copiedFaces = _faces;

		// compute face and vertex normals for each morph
		for (size_t i=0, il=_morphTargets.size(); i<il; i++) {

			// create on first access

      MorphTarget &mt = _morphTargets[i];
      MorphNormal &mn = _morphNormals[i];

			if (mn.isEmpty() ) {
				for (const Face3 &face : _faces) {
          mn.faceNormals.emplace_back();
					mn.vertexNormals.emplace_back();
				}
			}

			// compute morph normals
			computeFaceNormals(copiedFaces, mt.vertices);
			computeVertexNormals(copiedFaces, mt.vertices, true);

			// store morph normals
			for (size_t i=0, il=copiedFaces.size(); i<il; i++) {

        const Face3 &face = copiedFaces[i];
        mn.faceNormals[i] = face.normal;

        mn.vertexNormals[i][0] = face.vertexNormals[ 0 ];
        mn.vertexNormals[i][1] = face.vertexNormals[ 1 ];
        mn.vertexNormals[i][2] = face.vertexNormals[ 2 ];
			}
		}
    return *this;
	}

	StaticGeometry &computeLineDistances()
  {
		float d = 0;
		for (size_t i = 0, il = _vertices.size(); i < il; i ++ ) {
			if ( i > 0 ) {
				d += _vertices[ i ].distanceTo( _vertices[ i - 1 ] );
			}
			_lineDistances[ i ] = d;
		}
    return *this;
	}

	StaticGeometry &computeBoundingBox() override
  {
		_boundingBox.set(_vertices);
    return *this;
	}

	StaticGeometry &computeBoundingSphere() override
  {
		_boundingSphere.set(_vertices);
    return *this;
	}

	StaticGeometry &merge(const StaticGeometry &geometry, const math::Matrix4 &matrix, unsigned materialIndexOffset=0)
  { 
    unsigned vertexOffset = (unsigned)_vertices.size();
    std::vector<Vertex> &vertices1 = _vertices;
    const std::vector<Vertex> &vertices2 = geometry._vertices;
		auto &colors1 = _colors;
		auto &colors2 = geometry._colors;

    math::Matrix3 normalMatrix = matrix.normalMatrix();

		// vertices
		for (Vertex vertex : vertices2) {
			vertex.apply( matrix );
			vertices1.push_back(vertex);
		}

		// colors
		for (Color color : colors2) {
			colors1.push_back(color);
		}

		// faces
		for (const Face3 &face2 : geometry._faces) {
			Face3 face(face2.a + vertexOffset, face2.b + vertexOffset, face2.c + vertexOffset, face2.normal);

      face.normal.apply( normalMatrix ).normalize();

			for (unsigned i=0; i<3; i++) {
				face.vertexNormals[i].apply( normalMatrix ).normalize();
			}

			face.materialIndex = face.materialIndex + materialIndexOffset;

			_faces.push_back(face);
		}

		// uvs
		for (unsigned i = 0, il = (unsigned)geometry._faceVertexUvs[ 0 ].capacity(); i < il; i ++ )
    {
      _faceVertexUvs[0].push_back(geometry._faceVertexUvs[0][i]);
		}
    return *this;
	}

	/*
	 * Checks for duplicate vertices with hashmap.
	 * Duplicated vertices are removed
	 * and faces' vertices are updated.
	 */
	size_t mergeVertices()
  {
		unsigned precisionPoints = 4; // number of decimal points, e.g. 4 for epsilon of 0.0001
		double precision = std::pow( 10, precisionPoints );

    std::unordered_map<std::string, unsigned> verticesMap;
    std::vector<Vertex> unique;
    std::unordered_map<unsigned, unsigned> changes;

		for (unsigned i = 0, il = _vertices.size(); i < il; i ++ ) {
			const Vertex v = _vertices[ i ];


      std::stringstream ss;
      ss<< std::round( v.x() * precision ) << '_' << std::round( v.y() * precision ) << '_' << std::round( v.z() * precision );
      std::string key = ss.str();

			if (verticesMap.find(key) == verticesMap.end()) {
				verticesMap[key] = i;
				unique.push_back( _vertices[i] );
				changes[i] = unique.size() - 1;
			}
      else {
				//console.log('Duplicate vertex found. ', i, ' could be using ', verticesMap[key]);
				changes[ i ] = changes[ verticesMap[ key ] ];
			}
		}

		// if faces are completely degenerate after merging vertices, we
		// have to remove them from the geometry.
		std::vector<unsigned> faceIndicesToRemove;

    std::array<int, 3> indices;

		for (unsigned i = 0, il = _faces.size(); i < il; i ++ ) {

			Face3 &face = _faces[i];

			face.a = changes[ face.a ];
			face.b = changes[ face.b ];
			face.c = changes[ face.c ];

			indices = {face.a, face.b, face.c};

			// if any duplicate vertices are found in a Face3
			// we have to remove the face as nothing can be saved
			for (unsigned n = 0; n < 3; n ++ ) {

				if ( indices[ n ] == indices[ ( n + 1 ) % 3 ] ) {
					faceIndicesToRemove.push_back( i );
					break;
				}
			}
		}

		for (unsigned i = faceIndicesToRemove.size() - 1; i >= 0; i--) {

			unsigned idx = faceIndicesToRemove[i];

			_faces.erase(_faces.begin()+idx);

			for (unsigned j = 0, jl = _faceVertexUvs.size(); j < jl; j ++ ) {
				_faceVertexUvs[j].erase(_faceVertexUvs[j].begin()+idx);
			}
		}

		// Use unique set of vertices
		auto diff = _vertices.size() - unique.size();
		_vertices = unique;
		return diff;

	}

	void sortFacesByMaterialIndex()
  {
    // tag faces
    std::unordered_map<unsigned, unsigned> idMap;
    for(unsigned i=0; i<_faces.size(); i++) {
      idMap[_faces[i].materialIndex] = i;
    }

    // sort faces
    std::sort(_faces.begin(), _faces.end(), [](const Face3 &f1, const Face3& f2) {
      return f1.materialIndex < f2.materialIndex;
    });

		// sort uvs
		auto &uvs1 = _faceVertexUvs[ 0 ];
		auto &uvs2 = _faceVertexUvs[ 1 ];

    std::vector<std::array<UV, 3>> newUvs1, newUvs2;

		if (uvs1.size() == _faces.size()) {
      for(const Face3 &face : _faces) {
        unsigned id = idMap[face.materialIndex];
        newUvs1.push_back( uvs1[ id ] );
      }
    }
		if (uvs2.size() == _faces.size()) {
      for(const Face3 &face : _faces) {
        unsigned id = idMap[face.materialIndex];
        newUvs2.push_back( uvs2[ id ] );
      }
    }

		_faceVertexUvs[ 0 ] = newUvs1;
		_faceVertexUvs[ 1 ] = newUvs2;

	}

  void addFacre(const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t materialIndex,
               const BufferAttribute<float>::Ptr &normals, const BufferAttribute<float>::Ptr &uvs,
               const BufferAttribute<float>::Ptr &uv2s,
               const std::vector<Vertex> &tempNormals, const std::vector<UV> &tempUVs,
               const std::vector<UV> &tempUVs2)
  {
  }

  StaticGeometry &set(const BufferGeometry &geometry )
  {
    std::vector<Vertex> tempNormals;
    std::vector<UV> tempUVs;
    std::vector<UV> tempUVs2;

    const BufferAttribute<float>::Ptr &positions = geometry.position();
    const BufferAttribute<float>::Ptr &normals = geometry.normal();
    const BufferAttribute<float>::Ptr &colors = geometry.color();
    const BufferAttribute<float>::Ptr &uvs = geometry.uv();
    const BufferAttribute<float>::Ptr &uv2s = geometry.uv2();

    for(size_t i = 0, j = 0; i < positions->size(); i += 3, j += 2 ) {

      _vertices.emplace_back((*positions)[i], (*positions)[i + 1], (*positions)[i + 2]);

      if(normals) {
        tempNormals.emplace_back((*normals)[i], (*normals)[i + 1], (*normals)[i + 2]);
      }

      if(colors) {
        _colors.emplace_back((*colors)[i], (*colors)[i + 1], (*colors)[i + 2]);
      }

      if(uvs) {
        tempUVs.emplace_back((*uvs)[j], (*uvs)[j + 1]);
      }

      if(uv2s) {
        tempUVs2.emplace_back((*uv2s)[j], (*uv2s)[j + 1]);
      }
    }

    auto addFace = [&](const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t materialIndex)
    {
      std::array<Vertex, 3> vertexNormals;
      if(normals) vertexNormals = {tempNormals[a], tempNormals[b], tempNormals[c]};

      std::array<Color, 3> vertexColors;
      if(colors) vertexColors = {_colors[a], _colors[b], _colors[c]};

      Face3 face(a, b, c, vertexNormals, vertexColors, materialIndex);

      _faces.push_back(face);

      if (uvs) {
        _faceVertexUvs[0].push_back({tempUVs[a], tempUVs[b], tempUVs[c]});
      }

      if (uv2s) {
        _faceVertexUvs[1].push_back({tempUVs2[a], tempUVs2[b], tempUVs2[c]});
      }
    };

    const BufferAttribute<uint32_t>::Ptr &indices = geometry.index();

    if (geometry.groups().size() > 0 ) {

      for(const Group &group : geometry.groups()) {

        for (auto j = group.start, jl = group.start + group.count; j < jl; j += 3 ) {
          if (indices)
            addFace((*indices)[j], (*indices)[j + 1], (*indices)[j + 2], group.materialIndex);
          else
            addFace(j, j + 1, j + 2, group.materialIndex);
        }
      }
    }
    else {
      if (indices) {
        for (size_t i = 0; i < indices->size(); i += 3 ) {
          addFace( (*indices)[ i ], (*indices)[ i + 1 ], (*indices)[ i + 2 ], 0);
        }
      } else {
        for (size_t i = 0; i < positions->size() / 3; i += 3 ) {
          addFace( i, i + 1, i + 2, 0);
        }
      }
    }

    computeFaceNormals(_faces, _vertices);

    _boundingBox = geometry.boundingBox();

    _boundingSphere = geometry.boundingSphere();

    return *this;
  }

  void raycast(const Line &line,
               const Raycaster &raycaster, const math::Ray &ray,
               std::vector<Intersection> &intersects) override;

  void raycast(const Mesh &mesh,
               const Raycaster &raycaster,
               const math::Ray &ray,
               math::Vector3 &intersectionPoint,
               math::Vector3 &intersectionPointWorld,
               std::vector<Intersection> &intersects) override;
};

} //three

#endif //THREE_QT_STATICGEOMETRY_H
