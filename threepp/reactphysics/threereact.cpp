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

  // Compute the transform used for rendering the box
  rp3d::decimal matrix[16];
  interpolatedTransform.getOpenGLMatrix(matrix);
  math::Matrix4 newMatrix(matrix[0], matrix[4], matrix[8], matrix[12],
                          matrix[1], matrix[5], matrix[9], matrix[13],
                          matrix[2], matrix[6], matrix[10], matrix[14],
                          matrix[3], matrix[7], matrix[11], matrix[15]);

  // Apply the scaling matrix to have the correct box dimensions
  object->setRotationFromMatrix(newMatrix * _scalingMatrix);
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
  _interpolationFactor = _timer.computeInterpolationFactor();

  _dynamicsWorld->update(_timer.timeStep());

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

rp3d::RigidBody *PhysicsScene::getBody(Object3D::Ptr object, bool addShapes)
{
  auto found = _objects.find(object.get());
  if(found != _objects.end()) return found->second._body;

  {
    std::lock_guard<std::mutex> lck(_createBodyMutex);

    auto found = _objects.find(object.get());
    if(found != _objects.end()) return found->second._body;

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
      else if(CAST(object->geometry(), geom, LinearGeometry)) {

      }
      else if(CAST(object->geometry(), geom, BufferGeometry)) {

      }
    }

    return body;
  }
}

}
}