//
// Created by byter on 22.08.17.
//

#ifndef THREEPP_BUFFERGEOMETRY_H
#define THREEPP_BUFFERGEOMETRY_H

#include <unordered_map>
#include <functional>
#include <threepp/util/Types.h>
#include <threepp/util/osdecl.h>
#include "Geometry.h"
#include "BufferAttribute.h"

namespace three {
enum class IndexedAttributeName : size_t
{
  morphTarget, morphNormal
};
using IndexedAttributeKey = std::pair<IndexedAttributeName, size_t>;
}

namespace std
{
template<> struct hash<three::IndexedAttributeKey>
{
  typedef three::IndexedAttributeKey argument_type;
  typedef std::size_t result_type;
  result_type operator()(argument_type const& s) const noexcept
  {
    auto h = three::EnumHash{}(s.first);
    three::hash_combine(h, s.second);
    return h;
  }
};
}

namespace three {

class Object3D;
class LinearGeometry;
class DirectGeometry;

enum class AttributeName
{
  index, color, position, normal, uv, uv2, lineDistances, unknown
};

class DLX BufferGeometry : public Geometry
{
  friend class BufferGeometryAccess;

  BufferAttributeT<uint32_t>::Ptr _index;
  BufferAttributeT<float>::Ptr _position;
  BufferAttributeT<float>::Ptr _normal;
  BufferAttributeT<float>::Ptr _color;
  BufferAttributeT<float>::Ptr _uv;
  BufferAttributeT<float>::Ptr _uv2;
  BufferAttributeT<float>::Ptr _lineDistances;
  BufferAttributeT<float>::Ptr _tangents;
  BufferAttributeT<float>::Ptr _bitangents;

  std::vector<BufferAttributeT<float>::Ptr> _morphAttributes_position;
  std::vector<BufferAttributeT<float>::Ptr> _morphAttributes_normal;

  BufferAttributeT<float>::Ptr _skinIndices;
  BufferAttributeT<float>::Ptr _skinWeight;

  std::unordered_map<IndexedAttributeKey, BufferAttributeT<float>::Ptr> _indexedAttributes;

  UpdateRange _drawRange;

  void setFromLinearGeometry(const LinearGeometry &geometry);
  void setFromMeshGeometry(LinearGeometry &geometry);
  void setFromDirectGeometry(std::shared_ptr<DirectGeometry> geometry);

protected:
  BufferGeometry(const BufferGeometry &geom);

  BufferGeometry() : Geometry(geometry::Typer(this))
  {}

  BufferGeometry &computeBoundingBox() override
  {
    if (_position) {
      _boundingBox = _position->box3();
    }
    else {
      _boundingBox.makeEmpty();
    }
    return *this;
  }

  BufferGeometry &computeBoundingSphere() override
  {
    if (_position) {
      math::Box3 box = _position->box3();
      math::Vector3 center = box.getCenter();

      // hoping to find a boundingSphere with a radius smaller than the
      // boundingSphere of the boundingBox: sqrt(3) smaller in the best case
      float maxRadiusSq = 0;

      for (size_t i=0, il = _position->itemCount(); i < il; i ++ ) {
        math::Vector3 v(_position->get_x(i), _position->get_y(i), _position->get_z(i));
        maxRadiusSq = std::max(maxRadiusSq, center.distanceToSquared(v));
      }

      _boundingSphere = math::Sphere(center, std::sqrt(maxRadiusSq));
    }
    return *this;
  }

  BufferGeometry(std::shared_ptr<Object3D> object, LinearGeometry &geometry);

  BufferGeometry(std::shared_ptr<Object3D> object, std::shared_ptr<LinearGeometry> geometry, const geometry::Typer &typer)
     : BufferGeometry(object, *geometry)
  {
    Geometry::typer = typer;
    Geometry::typer.allow<BufferGeometry>();
  }

  explicit BufferGeometry(const geometry::Typer &typer) : Geometry(typer)
  {}

  BufferGeometry(const BufferGeometry &geom, const geometry::Typer &typer)
     : BufferGeometry(geom)
  {
    Geometry::typer = typer;
    Geometry::typer.allow<BufferGeometry>();
  }

  template <typename Sub>
  static Sub *setTyper(Sub *sub) {
    sub->typer = geometry::Typer(static_cast<BufferGeometry *>(sub));
    return sub;
  }

public:
  using Ptr = std::shared_ptr<BufferGeometry>;
  static Ptr make() {
    return Ptr(new BufferGeometry());
  }
  static Ptr make(std::shared_ptr<Object3D> object, LinearGeometry &geometry) {
    return Ptr(new BufferGeometry(object, geometry));
  }

  BufferGeometry *cloned() const override {
    return new BufferGeometry(*this);
  }

  BufferGeometry &operator =(const BufferGeometry &geom) {
    _index = geom._index;
    _position = geom._position;
    _normal = geom._normal;
    _color = geom._color;;
    _uv = geom._uv;
    _uv2 = geom._uv2;
    _lineDistances = geom._lineDistances;

    _morphAttributes_position = geom._morphAttributes_position;
    _morphAttributes_normal = geom._morphAttributes_normal;

    _indexedAttributes = geom._indexedAttributes;

    _drawRange = geom._drawRange;
    return *this;
  }

  BufferGeometry &update(std::shared_ptr<Object3D> object, LinearGeometry *geometry);

  void computeVertexNormals();

  void normalizeNormals();

  bool useMorphing() const override
  {
    return !_morphAttributes_position.empty();
  }

  const UpdateRange &drawRange() const {return _drawRange;}

  const BufferAttributeT<uint32_t>::Ptr &index() const {return _index;}

  BufferAttributeT<uint32_t>::Ptr &getIndex() {return _index;}

  const BufferAttributeT<float>::Ptr &position() const {return _position;}

  BufferAttributeT<float>::Ptr &getPosition() {return _position;}

  const BufferAttributeT<float>::Ptr &normal() const {return _normal;}

  const BufferAttributeT<float>::Ptr &color() const {return _color;}

  const BufferAttributeT<float>::Ptr &uv() const {return _uv;}

  const BufferAttributeT<float>::Ptr &uv2() const {return _uv2;}

  const BufferAttributeT<float>::Ptr tangents() const {return _tangents;}

  const BufferAttributeT<float>::Ptr bitangents() const {return _bitangents;}

  const std::vector<BufferAttributeT<float>::Ptr> &morphPositions() const {return _morphAttributes_position;}

  const std::vector<BufferAttributeT<float>::Ptr> &morphNormals() const {return _morphAttributes_normal;}

  void addAttribute(IndexedAttributeName attribute, size_t index, BufferAttributeT<float>::Ptr value) {
    IndexedAttributeKey key = {attribute, index};
    _indexedAttributes[key] = value;
  }

  void removeAttribute(IndexedAttributeName attribute, size_t index) {
    _indexedAttributes.erase({attribute, index});
  }

  void setDrawRange(size_t start, size_t count ) {

    _drawRange.offset = start;
    _drawRange.count = count;
  }

  BufferGeometry &setIndex(const BufferAttributeT<uint32_t>::Ptr &indices) {
    _index = indices;
    return *this;
  }

  BufferGeometry &setPosition(const BufferAttributeT<float>::Ptr &position)
  {
    _position = position;
    return *this;
  }

  BufferGeometry &setNormal(const BufferAttributeT<float>::Ptr &normal)
  {
    _normal = normal;
    return *this;
  }

  BufferGeometry &setColor(const BufferAttributeT<float>::Ptr &color)
  {
    _color = color;
    return *this;
  }

  BufferGeometry &setUV(const BufferAttributeT<float>::Ptr &uv)
  {
    _uv = uv;
    return *this;
  }

  BufferGeometry &setUV2(const BufferAttributeT<float>::Ptr &uv2)
  {
    _uv2 = uv2;
    return *this;
  }

  BufferGeometry &setTangents(const BufferAttributeT<float>::Ptr &tangents)
  {
    _tangents = tangents;
    return *this;
  }

  BufferGeometry &setBitangents(const BufferAttributeT<float>::Ptr &bitangents)
  {
    _bitangents = bitangents;
    return *this;
  }

  BufferGeometry &setLineDistances(const BufferAttributeT<float>::Ptr &lineDistances)
  {
    _lineDistances = lineDistances;
    return *this;
  }

  BufferAttribute::Ptr getAttribute(AttributeName name)
  {
    switch(name) {
      case AttributeName::normal:
        return _normal;
      case AttributeName::lineDistances:
        return _lineDistances;
      case AttributeName::index:
        return _index;
      case AttributeName::uv:
        return _uv;
      case AttributeName::uv2:
        return _uv2;
      case AttributeName::color:
        return _color;
      case AttributeName::position:
        return _position;
      default:
        return nullptr;
    }
  }

  BufferGeometry &apply(const math::Matrix4 &matrix) override
  {
    if (_position) {
      _position->apply(matrix);
      _position->needsUpdate();
    }

    if (_normal) {

      math::Matrix3 normalMatrix = matrix.normalMatrix();

      _normal->apply(normalMatrix);
      _normal->needsUpdate();
    }

    computeBoundingBox();
    computeBoundingSphere();

    return *this;
  }

  size_t vertexCount() const override {
    return _position->itemCount();
  }

  math::Vector3 centroid(const Face3 &face) const override;

  void raycast(Line &line,
               const Raycaster &raycaster,
               const std::vector<math::Ray> &ray,
               std::vector<Intersection> &intersects) override;

  void raycast(Mesh &mesh,
               const Raycaster &raycaster,
               const std::vector<math::Ray> &ray,
               std::vector<Intersection> &intersects) override;
};

class InstancedBufferGeometry : public BufferGeometry
{
  unsigned _maxInstancedCount = 0;

protected:
  explicit InstancedBufferGeometry()
     : BufferGeometry(geometry::Typer(this)) {}
  explicit InstancedBufferGeometry(std::shared_ptr<Object3D> object,
                                   std::shared_ptr<LinearGeometry> geometry)
     : BufferGeometry(object, geometry, geometry::Typer(this)) {}

public:
  using Ptr = std::shared_ptr<InstancedBufferGeometry>;
  static Ptr make() {
    return Ptr(new InstancedBufferGeometry());
  }
  static Ptr make(std::shared_ptr<Object3D> object, std::shared_ptr<LinearGeometry> geometry) {
    return Ptr(new InstancedBufferGeometry(object, geometry));
  }

  unsigned maxInstancedCount() const {return _maxInstancedCount;}

  InstancedBufferGeometry *cloned() const override
  {
    return BufferGeometry::setTyper(new InstancedBufferGeometry(*this));
  }
};

}

#endif //THREEPP_BUFFERGEOMETRY_H
