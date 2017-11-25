//
// Created by byter on 29.07.17.
//

#include "Object3D.h"
#include <math/Math.h>
#include <math/Quaternion.h>

namespace three {

using namespace three::math;

Object3D::Object3D(object::Resolver::Ptr resolver)
   : objectResolver(resolver), uuid(sole::uuid0()), _id(++__id_count)
{
}

void Object3D::applyMatrix(const Matrix4 &matrix)
{
  _matrix *= matrix;
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
  if (_matrixAutoUpdate) updateMatrix();

  if (_matrixWorldNeedsUpdate || force ) {

    if (_parent) {
      _matrixWorld = _parent->_matrixWorld * _matrix;
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
