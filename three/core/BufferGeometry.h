//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_BUFFERGEOMETRY_H
#define THREE_QT_BUFFERGEOMETRY_H

#include <unordered_map>
#include "Geometry.h"
#include "BufferAttribute.h"

namespace three {

class Object3D;

enum IndexedAttribute
{
  morphTarget, morphNormal
};
class BufferGeometry : public Geometry
{
  BufferAttribute<uint32_t>::Ptr _index;
  BufferAttribute<float>::Ptr _position;
  BufferAttribute<float>::Ptr _normal;
  BufferAttribute<float>::Ptr _color;
  BufferAttribute<float>::Ptr _uv;
  BufferAttribute<float>::Ptr _uv2;

  std::vector<BufferAttribute<float>::Ptr> _morphAttributes_position;
  std::vector<BufferAttribute<float>::Ptr> _morphAttributes_normal;

  std::unordered_map<std::pair<IndexedAttribute, size_t>, BufferAttribute<float>::Ptr> _indexedAttributes;
  //this.drawRange = { start: 0, count: Infinity };

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

  const BufferAttribute<uint32_t>::Ptr &index() const {return _index;}

  BufferAttribute<uint32_t>::Ptr &getIndex() {return _index;}

  const BufferAttribute<float>::Ptr &position() const {return _position;}

  const BufferAttribute<float>::Ptr &normal() const {return _normal;}

  const BufferAttribute<float>::Ptr &color() const {return _color;}

  const BufferAttribute<float>::Ptr &uv() const {return _uv;}

  const BufferAttribute<float>::Ptr &uv2() const {return _uv2;}

  const std::vector<BufferAttribute<float>::Ptr> &morphPositions() const {return _morphAttributes_position;}

  const std::vector<BufferAttribute<float>::Ptr> &morphNormals() const {return _morphAttributes_normal;}

  void addAttribute(IndexedAttribute attribute, size_t index, BufferAttribute<float>::Ptr value) {
    _indexedAttributes.insert({attribute, index}, value);
  }

  void removeAttribute(IndexedAttribute attribute, size_t index) {
    _indexedAttributes.erase({attribute, index});
  }

  void setIndex(const std::vector<uint32_t> &indices) {
    if(_index)
      _index->set(indices);
    else
      _index = BufferAttribute<uint32_t>::make(indices, 3);
  }

  BufferGeometry &setPosition(const BufferAttribute<float>::Ptr &position)
  {
    _position = position;
    return *this;
  }

  BufferGeometry &setNormal(const BufferAttribute<float>::Ptr &normal)
  {
    _normal = normal;
    return *this;
  }

  BufferGeometry &setColor(const BufferAttribute<float>::Ptr &color)
  {
    _color = color;
    return *this;
  }

  BufferGeometry &setUV(const BufferAttribute<float>::Ptr &uv)
  {
    _uv = uv;
    return *this;
  }

  BufferGeometry &setUV2(const BufferAttribute<float>::Ptr &uv2)
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

}

#endif //THREE_QT_BUFFERGEOMETRY_H
