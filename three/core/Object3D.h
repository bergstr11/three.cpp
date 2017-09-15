//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_OBJECT
#define THREEQT_OBJECT

#include <vector>
#include <memory>
#include <functional>

#include "core/StaticGeometry.h"
#include "math/Euler.h"
#include "math/Quaternion.h"
#include "math/Matrix4.h"
#include "material/Material.h"
#include "Layers.h"

namespace three {

class Raycaster;
class Intersection;

class Object3D
{
public:
  using Ptr = std::shared_ptr<Object3D>;

private:
  std::string _name;
  uint16_t _id;

  Object3D *_parent;
  std::vector<Ptr> _children;

protected:
  math::Vector3 _up {0, 1, 0};
  math::Vector3 _position;
  math::Euler _rotation;
  math::Quaternion _quaternion;
  math::Vector3 _scale {1, 1, 1};

  math::Matrix4 _matrix = math::Matrix4::identity();
  math::Matrix4 _matrixWorld = math::Matrix4::identity();

  std::vector<Material::Ptr> _materials;

  bool _matrixAutoUpdate = true;
  bool _matrixWorldNeedsUpdate = false;

  Layers _layers;
  bool _visible = true;

  bool _castShadow = false;
  bool _receiveShadow = false;

  bool _frustumCulled = true;
  int _renderOrder = -1;

  Geometry::Ptr _geometry;

protected:
  Object3D();
  Object3D(Geometry::Ptr geometry);

public:
  uint16_t id() const {return _id;}
  const Layers &layers() const {return _layers;}
  const Geometry::Ptr geometry() const {return _geometry;}
  const math::Matrix4 &matrix() const {return _matrix;}
  const bool matrixAutoUpdate() const {return _matrixAutoUpdate;}

  void applyMatrix(const math::Matrix4 &matrix);

  bool visible() const {return _visible;}

  const std::vector<Ptr> &children() const {return _children;}

  math::Vector3 &position() {return _position;}
  math::Euler &rotation() {return _rotation;}
  math::Matrix4 &matrixWorld() {return _matrixWorld;}
  Material::Ptr material() {return _materials.empty() ? nullptr : _materials.at(0);}

  const math::Vector3 &position() const {return _position;}
  const math::Euler &rotation() const {return _rotation;}
  const math::Matrix4 &matrixWorld() const {return _matrixWorld;}
  const Material::Ptr material() const {return _materials.empty() ? nullptr : _materials.at(0);}

  const Object3D *parent() const {return _parent;}

  int renderOrder() const {return _renderOrder;}

  const std::vector<Material::Ptr> materials() const {return _materials;}
  const Material::Ptr material(unsigned index) const {
    return _materials.size() > index ? _materials.at(index) : nullptr;
  }

  void applyQuaternion(math::Quaternion q)
  {
    _quaternion *= q;
  }

  void setRotationFromAxisAngle(const math::Vector3 &axis, float angle )
  {
    // assumes axis is normalized
    _quaternion.set( axis, angle );
  }

  void setRotationFromEuler(const math::Euler &euler)
  {
    _quaternion = euler.toQuaternion();
  }

  void setRotationFromMatrix(const math::Matrix4 &m)
  {
    // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)
    _quaternion.set(m);

  }

  void setRotationFromQuaternion(const math::Quaternion &q)
  {
    // assumes q is normalized
    _quaternion = q;
  }

  Object3D &rotateOnAxis(const math::Vector3 &axis, float angle)
  {
    // rotate object on axis in object space
    // axis is assumed to be normalized
    _quaternion *= math::Quaternion(axis, angle);
    return *this;
  }

  void rotateX(float angle)
  {
    rotateOnAxis(math::Vector3( 1, 0, 0 ), angle );
  }

  void rotateY(float angle)
  {
    rotateOnAxis(math::Vector3( 0, 1, 0 ), angle);
  }

  void rotateZ(float angle)
  {
    rotateOnAxis(math::Vector3( 0, 0, 1 ), angle);
  }

  Object3D &translateOnAxis(const math::Vector3 &axis, float distance)
  {
    // translate object by distance along axis in object space
    // axis is assumed to be normalized

    math::Vector3 v( axis );
    v.apply(_quaternion);

    _position += (v *= distance);

    return *this;
  }

  void translateX(float distance)
  {
    translateOnAxis(math::Vector3( 1, 0, 0 ), distance );
  }

  void translateY(float distance)
  {
    translateOnAxis(math::Vector3( 0, 1, 0 ), distance );
  }

  void translateZ(float distance)
  {
    translateOnAxis(math::Vector3( 0, 0, 1 ), distance );
  }

  math::Vector3 localToWorld(const math::Vector3 &vector)
  {
    return vector * _matrixWorld;
  }

  math::Vector3 worldToLocal(const math::Vector3 &vector)
  {
    return vector * _matrixWorld.inverse();
  }

  virtual void lookAt(const math::Vector3 &vector)
  {
    // This method does not support objects with rotated and/or translated parent(s)
    math::Matrix4 m1( vector, _position, _up );

    _quaternion.set(m1);
  }

  void add(Object3D::Ptr object)
  {
    if (object.get() == this ) {
      return;
    }

    if ( object->_parent) {
      object->_parent->remove(object);
    }

    object->_parent = this;
    //object->dispatchEvent( 'added');

    _children.push_back( object );
  }

  void remove(Object3D::Ptr object)
  {
    auto index = std::find(_children.begin(), _children.end(), object);

    if (index != _children.end()) {

      object->_parent = nullptr;

      //object.dispatchEvent( 'removed' );

      _children.erase(index);
    }
  }

  Object3D *getObjectByName(std::string name) {

    if(_name == name) return this;

    for (const auto &child : _children) {

      Object3D *object = child->getObjectByName(name);

      if (object) return object;
    }

    return nullptr;
  }

  math::Vector3 getWorldPosition()
  {
    updateMatrixWorld( true );
    return math::Vector3::fromMatrixPosition(_matrixWorld);
  }

  math::Quaternion getWorldQuaternion() const;

  math::Euler getWorldRotation() const;

  math::Vector3 getWorldScale() const;

  virtual math::Vector3 getWorldDirection() const;

  void traverse(std::function<void(Object3D &)> callback);

  void traverseVisible(std::function<void(Object3D &)> callback);

  void traverseAncestors(std::function<void(Object3D &)> callback);

  void updateMatrix();

  void updateMatrixWorld(bool force=false);

  virtual void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const {};
};

}

#endif //THREEQT_OBJECT
