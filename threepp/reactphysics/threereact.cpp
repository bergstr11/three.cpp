//
// Created by byter on 10/24/18.
//

#include "threereact.h"
#include <threepp/geometry/Box.h>

namespace three {
namespace react3d {

void PhysicsObject::updateTransform(Object3D *object, float interpolationFactor)
{
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
  for(const auto &joint : _joints) {
    _dynamicsWorld->destroyJoint(joint);
  }
  for(const auto &object : _objects) {
    _dynamicsWorld->destroyRigidBody(object.second._body);
  }
  delete _dynamicsWorld;
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

PhysicsObject &PhysicsScene::getPhysics(Object3D::Ptr object, bool addShapes)
{
  auto found = _objects.find(object.get());
  if(found != _objects.end()) return found->second;

  {
    std::lock_guard<std::mutex> lck(_createBodyMutex);

    auto found = _objects.find(object.get());
    if(found != _objects.end()) return found->second;

    auto wp = object->getWorldPosition();
    auto wq = object->getWorldQuaternion();

    rp3d::Vector3 rp(wp.x(), wp.y(), wp.z());
    rp3d::Quaternion rq(wq.x(), wq.y(), wq.z(), wq.w());

    rp3d::Transform transform(rp, rq);
    rp3d::RigidBody *body = _dynamicsWorld->createRigidBody(transform);
    _objects.emplace(object.get(), PhysicsObject(body, transform));

    if(addShapes) {
      if(CAST(object->geometry(), geom, geometry::BoxParams)) {
        //body->addCollisionShape();
      }
      else if(LinearGeometry *geom = object->geometry()->typer) {

      }
      else if(BufferGeometry *geom = object->geometry()->typer) {

      }
    }

    return _objects.at(object.get());
  }
}

}
}