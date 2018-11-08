//
// Created by byter on 10/21/18.
//

#ifndef THREE_PP_MODEL_H
#define THREE_PP_MODEL_H

#include <threepp/core/Object3D.h>
#include <threepp/scene/Scene.h>
#include <threepp/objects/Mesh.h>
#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/engine/Timer.h>
#include <unordered_map>
#include <set>
#include <mutex>

namespace reactphysics3d {
  class RigidBody;
}

namespace three {
namespace react3d {

class PhysicsObject
{
  friend class PhysicsScene;

  math::Box3 _bodyBox;
  math::Vector3 _boxPosition;
  Object3D::Ptr _object;

  rp3d::RigidBody * _body;
  rp3d::Transform _previousTransform;

  math::Matrix4 _scalingMatrix = math::Matrix4::identity();

public:
  PhysicsObject(Object3D::Ptr object) : _object(object) {}

  void updateTransform(float interpolation);

  void updateFromObject();

  rp3d::BoxShape *createBoundingBoxShape();

  rp3d::RigidBody * body() const {return _body;}
  const three::math::Vector3 boxPosition(Object3D::Ptr object=nullptr);
};

class PhysicsScene
{
  rp3d::Timer _timer;

  rp3d::DynamicsWorld *_dynamicsWorld;

  std::mutex _createBodyMutex;

  std::unordered_map<Object3D *, PhysicsObject> _objects;
  std::set<rp3d::Joint*> _joints;

public:
  PhysicsScene(Scene::Ptr object, rp3d::DynamicsWorld *dynamicsWorld)
  : _dynamicsWorld(dynamicsWorld), _timer(1.0f / 60) {}
  ~PhysicsScene();

  using Ptr = std::shared_ptr<PhysicsScene>;
  static Ptr make(Scene::Ptr scene, rp3d::DynamicsWorld *dynamicsWorld) {
    return Ptr(new PhysicsScene(scene, dynamicsWorld));
  }

  rp3d::Timer &timer() {return _timer;}

  void reset();

  void update();

  rp3d::DynamicsWorld *dynamicsWorld() {return _dynamicsWorld;}

  rp3d::HingeJoint* createHingeJoint(const rp3d::HingeJointInfo& jointInfo);

  PhysicsObject *createPhysics(Object3D::Ptr object, rp3d::BodyType);

  PhysicsObject *getPhysics(Object3D::Ptr object);

  void destroy(Object3D::Ptr object);
  void destroy(rp3d::Joint *joint);
};

}
}

#endif //THREE_PP_MODEL_H
