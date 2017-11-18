//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_BUFFERGEOMETRY_H
#define THREE_QT_BUFFERGEOMETRY_H

#include <unordered_map>
#include <helper/utils.h>
#include "Geometry.h"
#include "BufferAttribute.h"

namespace three {

class Object3D;

enum class AttributeName
{
  morphTarget, morphNormal, color, position, lineDistances
};

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

  using IndexedAttributesKey = std::pair<AttributeName, size_t>;
  std::unordered_map<IndexedAttributesKey, BufferAttributeT<float>::Ptr, pair_hash> _indexedAttributes;

  UpdateRange _drawRange = {0, std::numeric_limits<int32_t>::infinity()};

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

  BufferGeometry &update(std::shared_ptr<Object3D> object);

  bool useMorphing() const override
  {
    return !_morphAttributes_position.empty();
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

  void addAttribute(AttributeName attribute, size_t index, BufferAttributeT<float>::Ptr value) {
    IndexedAttributesKey key = {attribute, index};
    _indexedAttributes[key] = value;
  }

  void addAttribute(AttributeName attribute, BufferAttributeT<float>::Ptr value) {
    IndexedAttributesKey key = {attribute, 0};
    _indexedAttributes[key] = value;
  }

  void removeAttribute(AttributeName attribute, size_t index) {
    _indexedAttributes.erase({attribute, index});
  }

  void setIndex(const std::vector<uint32_t> &indices) {
    if(_index)
      _index->set(indices);
    else
      _index = BufferAttributeT<uint32_t>::make(indices, 3);
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

#endif //THREE_QT_BUFFERGEOMETRY_H
