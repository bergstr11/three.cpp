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

enum class IndexedAttribute
{
  morphTarget, morphNormal
};

class BufferGeometry : public Geometry
{
  BufferAttributeBase<uint32_t>::Ptr _index;
  BufferAttributeBase<float>::Ptr _position;
  BufferAttributeBase<float>::Ptr _normal;
  BufferAttributeBase<float>::Ptr _color;
  BufferAttributeBase<float>::Ptr _uv;
  BufferAttributeBase<float>::Ptr _uv2;

  std::vector<BufferAttributeBase<float>::Ptr> _morphAttributes_position;
  std::vector<BufferAttributeBase<float>::Ptr> _morphAttributes_normal;

  using IndexedAttributesKey = std::pair<IndexedAttribute, size_t>;
  std::unordered_map<IndexedAttributesKey, BufferAttributeBase<float>::Ptr, pair_hash> _indexedAttributes;

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

  const BufferAttributeBase<uint32_t>::Ptr &index() const {return _index;}

  BufferAttributeBase<uint32_t>::Ptr &getIndex() {return _index;}

  const BufferAttributeBase<float>::Ptr &position() const {return _position;}

  const BufferAttributeBase<float>::Ptr &normal() const {return _normal;}

  const BufferAttributeBase<float>::Ptr &color() const {return _color;}

  const BufferAttributeBase<float>::Ptr &uv() const {return _uv;}

  const BufferAttributeBase<float>::Ptr &uv2() const {return _uv2;}

  const std::vector<BufferAttributeBase<float>::Ptr> &morphPositions() const {return _morphAttributes_position;}

  const std::vector<BufferAttributeBase<float>::Ptr> &morphNormals() const {return _morphAttributes_normal;}

  void addAttribute(IndexedAttribute attribute, size_t index, BufferAttributeBase<float>::Ptr value) {
    IndexedAttributesKey key = {attribute, index};
    _indexedAttributes[key] = value;
  }

  void removeAttribute(IndexedAttribute attribute, size_t index) {
    _indexedAttributes.erase({attribute, index});
  }

  void setIndex(const std::vector<uint32_t> &indices) {
    if(_index)
      _index->set(indices);
    else
      _index = BufferAttributeBase<uint32_t>::make(indices, 3);
  }

  BufferGeometry &setPosition(const BufferAttributeBase<float>::Ptr &position)
  {
    _position = position;
    return *this;
  }

  BufferGeometry &setNormal(const BufferAttributeBase<float>::Ptr &normal)
  {
    _normal = normal;
    return *this;
  }

  BufferGeometry &setColor(const BufferAttributeBase<float>::Ptr &color)
  {
    _color = color;
    return *this;
  }

  BufferGeometry &setUV(const BufferAttributeBase<float>::Ptr &uv)
  {
    _uv = uv;
    return *this;
  }

  BufferGeometry &setUV2(const BufferAttributeBase<float>::Ptr &uv2)
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
