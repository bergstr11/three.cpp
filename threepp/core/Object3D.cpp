//
// Created by byter on 29.07.17.
//

#include "Object3D.h"

namespace three {

using namespace three::math;

void Object3D::dispose()
{
  for(auto i=0; i<materialCount(); i++) {
    material(i)->dispose();
  }
  for(auto child : children()) {
    child->dispose();
  }
}

void Object3D::updateMaterials()
{
  for(unsigned i=0, l=materialCount(); i<l; i++) {
    if(material(i)->lights)
      material(i)->needsUpdate = true;
  }
  for(auto &child : _children) {
    child->updateMaterials();
  }
}

void Object3D::onRotationChange(const math::Euler &rotation)
{
  _quaternion.set(_rotation, false);
}

void Object3D::onQuaternionChange(const math::Quaternion &quaternion)
{
  _rotation.set(_quaternion, Euler::RotationOrder::Default, false);
}

Object3D::Object3D(object::Resolver::Ptr resolver)
   : objectResolver(resolver), uuid(sole::uuid0()), _id(++__id_count)
{
  _rotation.onChange.connect(*this, &Object3D::onRotationChange);
  _quaternion.onChange.connect(*this, &Object3D::onQuaternionChange);
}

void Object3D::apply(const Matrix4 &matrix)
{
  _matrix.multiply(matrix, _matrix);
  math::decompose(_matrix, _position, _quaternion, _scale );
}

Quaternion Object3D::getWorldQuaternion() const
{
  Vector3 position(0), scale(0);
  Quaternion target(0);

  math::decompose(_matrixWorld, position, target, scale);

  return target;
}

math::Euler Object3D::getWorldRotation() const
{
  Quaternion quaternion = getWorldQuaternion();

  return Euler(quaternion, _rotation.getRotationOrder());
}

Vector3 Object3D::getWorldScale() const
{
  math::Vector3 position(0), target(0);
  math::Quaternion quaternion(0);

  math::decompose(_matrixWorld, position, quaternion, target);

  return target;
}

Vector3 Object3D::getWorldDirection() const
{
  Quaternion quaternion = getWorldQuaternion();

  return Vector3( 0, 0, 1 ).apply(quaternion);
}

void Object3D::traverse(std::function<void(Object3D &)> callback)
{
  callback( *this );

  for (auto child : _children) {
    child->traverse( callback );
  }
}

void Object3D::traverseVisible(std::function<void(Object3D &)> callback)
{
  if(!_visible) return;

  callback( *this );

  for (auto child : _children) {
    child->traverseVisible( callback );
  }
}

void Object3D::traverseAncestors(std::function<void(Object3D &)> callback)
{
  if (_parent) {
    callback(*_parent);
    _parent->traverseAncestors( callback );
  }
}

void Object3D::updateMatrix()
{
  _matrix = Matrix4::rotation(_quaternion);
  _matrix.scale(_scale);
  _matrix.setPosition(_position);

  _matrixWorldNeedsUpdate = true;
}

void Object3D::updateMatrixWorld(bool force)
{
  if (matrixAutoUpdate) updateMatrix();

  if (_matrixWorldNeedsUpdate || force ) {

    if (_parent) {
      _matrixWorld.multiply(_parent->_matrixWorld, _matrix);
    } else {
      _matrixWorld = _matrix;
    }

    _matrixWorldNeedsUpdate = false;
    force = true;
  }

  // update children
  for (Object3D::Ptr child : _children) {
    child->updateMatrixWorld( force );
  }
}

}
