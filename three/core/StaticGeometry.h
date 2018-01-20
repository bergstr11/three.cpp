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
#include <helper/Types.h>

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
  friend class DirectGeometry;
  friend class BufferGeometry;

  std::vector<Vertex> _vertices;
  std::vector<Vertex> _normals;
  std::vector<Color> _colors; // one-to-one vertex colors, used in ParticleSystem, Line and Ribbon

  std::vector<Face3> _faces;

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

  static StaticGeometry &computeFaceNormals(std::vector<Face3> &faces, std::vector<Vertex> vertices);

  static StaticGeometry &computeVertexNormals(std::vector<Face3> &faces, std::vector<Vertex> vertices,
                                        bool areaWeighted=true);

protected:
  StaticGeometry() {}

public:
  using Ptr = std::shared_ptr<StaticGeometry>;

  bool useMorphing() const override
  {
    return !_morphTargets.empty();
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

	StaticGeometry &computeMorphNormals();

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

	StaticGeometry &merge(const StaticGeometry &geometry,
                        const math::Matrix4 &matrix, unsigned materialIndexOffset=0);

	/*
	 * Checks for duplicate vertices with hashmap.
	 * Duplicated vertices are removed
	 * and faces' vertices are updated.
	 */
	size_t mergeVertices();

	void sortFacesByMaterialIndex();

  void addFace(uint32_t a, uint32_t b, uint32_t c, uint32_t materialIndex,
               const BufferAttributeT<float>::Ptr &normals, const BufferAttributeT<float>::Ptr &uvs,
               const BufferAttributeT<float>::Ptr &uv2s,
               const std::vector<Vertex> &tempNormals, const std::vector<UV> &tempUVs,
               const std::vector<UV> &tempUVs2);

  StaticGeometry &set(const BufferGeometry &geometry );

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
