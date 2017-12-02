//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_BUFFERGEOMETRY_H
#define THREE_QT_BUFFERGEOMETRY_H

#include <unordered_map>
#include <functional>
#include <helper/utils.h>
#include "Geometry.h"
#include "BufferAttribute.h"

namespace three {

class Object3D;

enum class IndexedAttributeName
{
  morphTarget, morphNormal
};
enum class AttributeName
{
  index, color, position, normal, uv, uv2, lineDistances, unknown
};

using IndexedAttributeKey = std::pair<IndexedAttributeName, size_t>;

class StaticGeometry;

class BufferGeometry : public Geometry
{
  BufferAttributeT<uint32_t>::Ptr _index;
  BufferAttributeT<float>::Ptr _position;
  BufferAttributeT<float>::Ptr _normal;
  BufferAttributeT<float>::Ptr _color;
  BufferAttributeT<float>::Ptr _uv;
  BufferAttributeT<float>::Ptr _uv2;
  BufferAttributeT<float>::Ptr _lineDistances;

  std::vector<BufferAttributeT<float>::Ptr> _morphAttributes_position;
  std::vector<BufferAttributeT<float>::Ptr> _morphAttributes_normal;

  std::unordered_map<IndexedAttributeKey, BufferAttributeT<float>::Ptr, pair_hash> _indexedAttributes;

  UpdateRange _drawRange;

protected:
  BufferGeometry &computeBoundingBox() override
  {
    if (_position) {
      _boundingBox = _position->box3();
    }
    else {
      _boundingBox.makeEmpty();
    }
  }

  BufferGeometry &computeBoundingSphere() override
  {
    //var box = new Box3();
    //var vector = new Vector3();

    if (_position) {
      math::Box3 box = _position->box3();
      math::Vector3 center = box.getCenter();

      // hoping to find a boundingSphere with a radius smaller than the
      // boundingSphere of the boundingBox: sqrt(3) smaller in the best case
      float maxRadiusSq = 0;

      for (size_t i=0, il = _position->count(); i < il; i ++ ) {
        math::Vector3 v(_position->get_x(i), _position->get_y(i), _position->get_z(i));
        maxRadiusSq = std::max(maxRadiusSq, center.distanceToSquared(v));
      }

      _boundingSphere = math::Sphere(center, std::sqrt(maxRadiusSq));
    }
  }

  explicit BufferGeometry() {}
  explicit BufferGeometry(std::shared_ptr<Object3D> object);

public:
  static size_t MaxIndex;

  using Ptr = std::shared_ptr<BufferGeometry>;
  static Ptr make() {
    return Ptr(new BufferGeometry());
  }
  static Ptr make(std::shared_ptr<Object3D> object) {
    return Ptr(new BufferGeometry(object));
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
  }

  BufferGeometry &update(std::shared_ptr<Object3D> object, std::shared_ptr<StaticGeometry> geometry);

  bool useMorphing() const override
  {
    return !_morphAttributes_position.empty();
  }

  void toBufferGeometry(BufferGeometry &geometry) override
  {
    geometry = *this;
  }

  const UpdateRange &drawRange() const {return _drawRange;}

  const BufferAttributeT<uint32_t>::Ptr &index() const {return _index;}

  BufferAttributeT<uint32_t>::Ptr &getIndex() {return _index;}

  const BufferAttributeT<float>::Ptr &position() const {return _position;}

  const BufferAttributeT<float>::Ptr &normal() const {return _normal;}

  const BufferAttributeT<float>::Ptr &color() const {return _color;}

  const BufferAttributeT<float>::Ptr &uv() const {return _uv;}

  const BufferAttributeT<float>::Ptr &uv2() const {return _uv2;}

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

  void setIndex(const std::vector<uint32_t> &indices) {
    if(_index)
      _index->set(indices);
    else
      _index = BufferAttributeT<uint32_t>::make(indices, 3, true);
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

class InstancedBufferGeometry : public BufferGeometry
{
  unsigned _maxInstancedCount = 0;

protected:
  explicit InstancedBufferGeometry() : BufferGeometry() {}
  explicit InstancedBufferGeometry(std::shared_ptr<Object3D> object) : BufferGeometry(object) {}

public:
  using Ptr = std::shared_ptr<InstancedBufferGeometry>;
  static Ptr make() {
    return Ptr(new InstancedBufferGeometry());
  }
  static Ptr make(std::shared_ptr<Object3D> object) {
    return Ptr(new InstancedBufferGeometry(object));
  }

  unsigned maxInstancedCount() const {return _maxInstancedCount;}
};

}

namespace std
{
template<> struct hash<three::IndexedAttributeKey>
{
  typedef three::IndexedAttributeKey argument_type;
  typedef std::size_t result_type;
  result_type operator()(argument_type const& s) const noexcept
  {
    result_type const h1 ( std::hash<three::IndexedAttributeName>{}(s.first) );
    result_type const h2 ( std::hash<size_t>{}(s.second) );
    return h1 ^ (h2 << 1);
  }
};
}

#endif //THREE_QT_BUFFERGEOMETRY_H
