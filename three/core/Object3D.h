//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_OBJECT
#define THREEQT_OBJECT

#include <vector>
#include <memory>
#include <functional>
#include <tuple>
#include <array>

#include <helper/sole.h>
#include <helper/simplesignal.h>
#include <math/Euler.h>
#include <math/Quaternion.h>
#include <math/Matrix4.h>
#include <material/Material.h>
#include <renderers/Resolver.h>
#include "StaticGeometry.h"
#include "Layers.h"

namespace three {

class Renderer;
class Raycaster;
class Intersection;
class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using CameraPtr = std::shared_ptr<Camera>;

class Object3D
{
public:
  const sole::uuid uuid;
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

  bool _matrixAutoUpdate = true;
  bool _matrixWorldNeedsUpdate = false;

  Layers _layers;
  bool _visible = true;

  int _renderOrder = -1;

  Geometry::Ptr _geometry;

protected:
  static const object::ResolverBase::Ptr defaultResolver() {return object::Resolver<void>::make();}

  Object3D(const object::ResolverBase::Ptr &resolver=defaultResolver());
  Object3D(const Geometry::Ptr &geometry, const object::ResolverBase::Ptr &resolver=defaultResolver());

public:
  const object::ResolverBase::Ptr resolver;

  Signal<void(Renderer &renderer, ScenePtr scene, CameraPtr camera, Geometry::Ptr geometry, Material::Ptr material, Group *group)> onBeforeRender;
  Signal<void(Renderer &renderer, ScenePtr scene, CameraPtr camera, Geometry::Ptr geometry, Material::Ptr material, Group *group)> onAfterRender;

  math::Matrix4 modelViewMatrix;
  math::Matrix3 normalMatrix;

  bool castShadow = false;
  bool receiveShadow = false;
  bool frustumCulled = true;

  Material::Ptr customDepthMaterial;
  Material::Ptr customDistanceMaterial;

  uint16_t id() const {return _id;}
  const Layers &layers() const {return _layers;}
  const Geometry::Ptr geometry() const {return _geometry;}
  const math::Matrix4 &matrix() const {return _matrix;}
  const bool matrixAutoUpdate() const {return _matrixAutoUpdate;}

  math::Matrix4 &matrix() {return _matrix;}

  void applyMatrix(const math::Matrix4 &matrix);

  virtual bool skinned() {return false;}

  bool visible() const {return _visible;}

  const std::vector<Ptr> &children() const {return _children;}

  math::Vector3 &up() {return _up;}
  math::Vector3 &position() {return _position;}
  math::Euler &rotation() {return _rotation;}
  math::Matrix4 &matrixWorld() {return _matrixWorld;}
  math::Quaternion &quaternion() {return _quaternion;}
  math::Vector3 &scale() {return _scale;}

  const math::Vector3 &position() const {return _position;}
  const math::Euler &rotation() const {return _rotation;}
  const math::Matrix4 &matrixWorld() const {return _matrixWorld;}
  const math::Quaternion &quaternion() const {return _quaternion;}
  const math::Vector3 &scale() const {return _scale;}

  const Object3D *parent() const {return _parent;}

  int renderOrder() const {return _renderOrder;}

  virtual bool renderable() const {return false;}

  virtual const Material::Ptr material() const {return nullptr;}
  virtual const Material::Ptr material(size_t index) const {return nullptr;}
  virtual const size_t materialCount() const {return 0;}

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

  virtual void updateMatrixWorld(bool force);

  virtual void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const {};
};

template <typename ... Mat>
class Object3DBase : public virtual Object3D
{
  std::tuple<std::shared_ptr<Mat> ...> _materialsTuple;
  std::array<Material::Ptr, sizeof ... (Mat)> _materialsArray;

protected:
  Object3DBase(const object::ResolverBase::Ptr &resolver, std::shared_ptr<Mat> ... args)
     : Object3D(resolver), _materialsTuple(args...), _materialsArray({args...}) {
  }
  Object3DBase(Geometry::Ptr geometry, const object::ResolverBase::Ptr &resolver, std::shared_ptr<Mat> ... args)
     : Object3D(geometry, resolver), _materialsTuple(args...), _materialsArray({args...}) {
  }

public:
  const Material::Ptr material() const override {return _materialsArray[0];}

  const Material::Ptr material(size_t index) const override {return _materialsArray[index];}

  const size_t materialCount() const override {return sizeof ... (Mat);}

  template<int N>
  decltype(std::get<N>(_materialsTuple)) material() {return std::get<N>(_materialsTuple);}

  size_t materialCount() {return sizeof ... (Mat);}
};

}

#endif //THREEQT_OBJECT
