//
// Created by byter on 10/24/18.
//

#include "threereact.h"
#include <threepp/geometry/Box.h>

namespace three {
namespace react3d {

rp3d::BoxShape *PhysicsObject::createBoundingBox(Object3D::Ptr object)
{
  const auto bb = object->computeBoundingBox();
  const auto bs = bb.getSize();

  _position = bb.getCenter();
  rp3d::BoxShape *box = new rp3d::BoxShape(rp3d::Vector3(bs.x(), bs.y(), bs.z()));
  _body->addCollisionShape(box, rp3d::Transform::identity(), 1.0f);

  return box;
}

void PhysicsObject::updateTransform(Object3D *object, float interpolationFactor)
{
  if(!_body) return;

  // Get the transform of the rigid body
  const rp3d::Transform &transform = _body->getTransform();

  // Interpolate the transform between the previous one and the new one
  rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(_previousTransform,
                                                                                 transform,
                                                                                 interpolationFactor);
  _previousTransform = transform;

  //world to local. We cannot set the world transform directly, unfortunately
  Object3D *p = object->parent();
  auto qinvers = p->getWorldQuaternion().inverse();

  const rp3d::Vector3 &pr = interpolatedTransform.getPosition();
  //math::Vector3 localPos = ((math::Vector3(pr.x, pr.y, pr.z) - p->getWorldPosition()) / p->getWorldScale()).apply(qinvers);

  const rp3d::Quaternion &qr = interpolatedTransform.getOrientation();
  three::math::Quaternion localQuat = qinvers * three::math::Quaternion(qr.x, qr.y, qr.z, qr.w);

  // Apply the scaling matrix to have the correct box dimensions
  //auto mx = newMatrix * _scalingMatrix;

  object->position() = p->worldToLocal(math::Vector3(pr.x, pr.y, pr.z));
  object->setRotationFromQuaternion(localQuat);
  object->updateMatrixWorld(true);
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

void PhysicsScene::update()
{
  _timer.update();

  while(_timer.isPossibleToTakeStep()) {

    // Take a physics simulation step
    _dynamicsWorld->update(_timer.getTimeStep());

    // Update the timer
    _timer.nextStep();
  }
  _interpolationFactor = _timer.computeInterpolationFactor();

  for(auto &object : _objects) {
    object.second.updateTransform(object.first, _interpolationFactor);
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

  auto wp = object->getWorldPosition();
  auto wq = object->getWorldQuaternion();

  rp3d::Vector3 rp(wp.x(), wp.y(), wp.z());
  rp3d::Quaternion rq(wq.x(), wq.y(), wq.z(), wq.w());

  rp3d::Transform transform(rp, rq);
  rp3d::RigidBody *body = _dynamicsWorld->createRigidBody(transform);
  body->setType(bodyType);
  _objects.emplace(object.get(), PhysicsObject(body, transform));

  PhysicsObject &physics = _objects.at(object.get());

  return &physics;
}

void PhysicsScene::remove(Object3D::Ptr object)
{
  auto found = _objects.find(object.get());
  if (found != _objects.end()) {
    _dynamicsWorld->destroyRigidBody(found->second._body);
    _objects.erase(found);
  }
}

void PhysicsScene::remove(rp3d::Joint *joint)
{
  if(_joints.count(joint)) {
    _joints.erase(joint);
    _dynamicsWorld->destroyJoint(joint);
  }
}

}
}