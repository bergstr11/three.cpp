//
// Created by byter on 10/24/18.
//

#include "threereact.h"
#include <threepp/geometry/Box.h>

namespace three {
namespace react3d {

rp3d::BoxShape *PhysicsObject::createBoundingBoxShape()
{
  const auto bs = _bodyBox.getSize();
  rp3d::BoxShape *box = new rp3d::BoxShape(rp3d::Vector3(bs.x(), bs.y(), bs.z()));
  _body->addCollisionShape(box, rp3d::Transform::identity(), 1.0f);

  return box;
}

const math::Vector3 PhysicsObject::boxPosition(Object3D::Ptr object)
{
  if(!object || object == _object) return _boxPosition;

  math::Vector3 pw = _object->localToWorld(_boxPosition);
  return object->worldToLocal(pw);
}

void PhysicsObject::updateFromObject()
{
  //update the transorm
  const auto wp = _object->getWorldPosition();
  auto wq = _object->getWorldQuaternion();
  rp3d::Vector3 rp(wp.x(), wp.y(), wp.z());
  rp3d::Quaternion rq(wq.x(), wq.y(), wq.z(), wq.w());
  _body->setTransform(rp3d::Transform(rp, rq));

  _body->setIsSleeping(false);

  // Reset the velocity of the rigid body
  _body->setLinearVelocity(rp3d::Vector3(0, 0, 0));
  _body->setAngularVelocity(rp3d::Vector3(0, 0, 0));

  _previousTransform = _body->getTransform();
}

void PhysicsObject::updateTransform(float interpolationFactor)
{
  if(_body->getType() == rp3d::BodyType::STATIC) return;

  // Get the transform of the rigid body
  const rp3d::Transform &transform = _body->getTransform();

  // Interpolate the transform between the previous one and the new one
  rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(_previousTransform,
                                                                                 transform,
                                                                                 interpolationFactor);
  _previousTransform = transform;

  Object3D *p = _object->parent();

  //set new position
  const rp3d::Vector3 &pr = interpolatedTransform.getPosition();
  _object->position() = p->worldToLocal(math::Vector3(pr.x, pr.y, pr.z));

  //We cannot set the world quaternion directly, unfortunately
  auto qinvers = p->getWorldQuaternion().inverse();
  const rp3d::Quaternion &qr = interpolatedTransform.getOrientation();
  three::math::Quaternion localQuat = qinvers * three::math::Quaternion(qr.x, qr.y, qr.z, qr.w);
  _object->setRotationFromQuaternion(localQuat);

  // Apply the scaling matrix to have the correct box dimensions
  //auto mx = newMatrix * _scalingMatrix;
}

PhysicsScene::~PhysicsScene()
{
  reset();
  delete _dynamicsWorld;
}

void PhysicsScene::reset()
{
  for(const auto &joint : _joints) {
    _dynamicsWorld->destroyJoint(joint);
  }
  _joints.clear();

  for(const auto &object : _objects) {
    if(object.second._body)
      _dynamicsWorld->destroyRigidBody(object.second._body);
  }
  _objects.clear();
}

void PhysicsScene::timedUpdate()
{
  if(!_timer.getIsRunning()) return;

  _timer.update();

  while(_timer.isPossibleToTakeStep()) {

    // Take a physics simulation step
    preUpdate();
    _dynamicsWorld->update(_timer.getTimeStep());

    // Update the timer
    _timer.nextStep();
  }
  float interpolationFactor = _timer.computeInterpolationFactor();

  for(auto &object : _objects) {
    object.second.updateTransform(interpolationFactor);
  }
}

void PhysicsScene::update(float seconds)
{
  unsigned times = seconds / _timer.getTimeStep();

  for(unsigned i=0; i<times; i++) {

    // Take a physics simulation step
    preUpdate();
    _dynamicsWorld->update(_timer.getTimeStep());
  }
  for(auto &object : _objects) {
    object.second.updateTransform(1.0f);
  }
}

rp3d::HingeJoint* PhysicsScene::createHingeJoint(const rp3d::HingeJointInfo& jointInfo)
{
  auto joint = dynamic_cast<rp3d::HingeJoint*>(_dynamicsWorld->createJoint(jointInfo));

  _joints.insert(joint);
  return joint;
}

PhysicsObject *PhysicsScene::getPhysics(Object3D::Ptr object)
{
  auto found = _objects.find(object.get());
  return (found != _objects.end()) ? &found->second : nullptr;
}

PhysicsObject *PhysicsScene::createPhysics(Object3D::Ptr object, rp3d::BodyType bodyType)
{
  std::lock_guard<std::mutex> lck(_createBodyMutex);

  auto found = _objects.find(object.get());
  if(found != _objects.end()) return &found->second;

  _objects.emplace(object.get(), PhysicsObject(object));
  PhysicsObject &physics = _objects.at(object.get());

  physics._bodyBox = object->computeBoundingBox();
  physics._boxPosition = object->worldToLocal(physics._bodyBox.getCenter());

  auto wp = object->getWorldPosition();
  auto wq = object->getWorldQuaternion();

  rp3d::Vector3 rp(wp.x(), wp.y(), wp.z());
  rp3d::Quaternion rq(wq.x(), wq.y(), wq.z(), wq.w());

  physics._previousTransform = rp3d::Transform(rp, rq);
  physics._body = _dynamicsWorld->createRigidBody(physics._previousTransform);
  physics._body->setType(bodyType);

  return &physics;
}

void PhysicsScene::destroy(Object3D::Ptr object)
{
  auto found = _objects.find(object.get());
  if (found != _objects.end()) {
    _dynamicsWorld->destroyRigidBody(found->second._body);
    _objects.erase(found);
  }
}

void PhysicsScene::destroy(rp3d::Joint *joint)
{
  if(_joints.count(joint)) {
    _joints.erase(joint);
    _dynamicsWorld->destroyJoint(joint);
  }
}

void HingeData::requestUpdate(const math::Quaternion &q)
{
  needsUpdate = true;
}

void HingeData::doUpdate()
{
  anchorPhysics->updateFromObject();
  elementPhysics->updateFromObject();

  needsUpdate = false;
}


}
}
