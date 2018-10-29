//
// Created by byter on 10/24/18.
//

#include "threereact.h"
#include <threepp/geometry/Box.h>

namespace three {
namespace react3d {

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
  math::Vector3 localPos = ((math::Vector3(pr.x, pr.y, pr.z) - p->getWorldPosition()) / p->getWorldScale()).apply(qinvers);

  const rp3d::Quaternion &qr = interpolatedTransform.getOrientation();
  three::math::Quaternion localQuat = qinvers * three::math::Quaternion(qr.x, qr.y, qr.z, qr.w);

  // Apply the scaling matrix to have the correct box dimensions
  //auto mx = newMatrix * _scalingMatrix;

  object->position() = localPos;
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
    _dynamicsWorld->update(_timer.timeStep());

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

PhysicsObject *PhysicsScene::getPhysics(Object3D::Ptr object, bool create, bool addShapes)
{
  auto found = _objects.find(object.get());
  if (found != _objects.end()) return &found->second;

  return create ? createPhysics(object, addShapes) : nullptr;
}

PhysicsObject *PhysicsScene::createPhysics(Object3D::Ptr object, bool addShapes)
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
  _objects.emplace(object.get(), PhysicsObject(body, transform));

  if(addShapes) {
    if(CAST(object->geometry(), geom, geometry::BoxParams)) {
      //TODO body->addCollisionShape();
    }
    else if(LinearGeometry *geom = object->geometry()->typer) {

    }
    else if(BufferGeometry *geom = object->geometry()->typer) {

    }
  }

  return &_objects.at(object.get());
}

rp3d::RigidBody *PhysicsScene::getPhysics(Object3D::Ptr object,
                                          const std::string &extraName,
                                          const rp3d::Vector3 &hingePosition,
                                          rp3d::CollisionShape *shape,
                                          const rp3d::Transform &shapeTransform,
                                          rp3d::decimal shapeMass)
{
  PhysicsObject *physics = getPhysics(object);
  if(!physics) {
    _objects.emplace(object.get(), PhysicsObject(nullptr, rp3d::Transform::identity()));
    physics = &_objects.at(object.get());
  }

  auto wq = object->getWorldQuaternion();
  rp3d::Quaternion rq(wq.x(), wq.y(), wq.z(), wq.w());

  rp3d::Transform trBody(hingePosition, rq);
  rp3d::RigidBody *extraBody = _dynamicsWorld->createRigidBody(trBody);

  extraBody->addCollisionShape(shape, shapeTransform, shapeMass);
  physics->_extraBodies[extraName] = extraBody;

  return extraBody;
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