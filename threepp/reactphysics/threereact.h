//
// Created by byter on 10/21/18.
//

#ifndef THREE_PP_MODEL_H
#define THREE_PP_MODEL_H

#include <threepp/core/Object3D.h>
#include <threepp/scene/Scene.h>
#include <threepp/objects/Mesh.h>
#include <reactphysics3d/reactphysics3d.h>
#include <unordered_map>
#include <mutex>

namespace reactphysics3d {
  class RigidBody;
}

namespace three {
namespace react3d {

struct PhysicsObject
{
  rp3d::RigidBody * const body;
  rp3d::Transform previousTransform;

  PhysicsObject(rp3d::RigidBody * body, const rp3d::Transform &transform)
     : body(body), previousTransform(transform)
  {}
};

class PhysicsScene
{
  rp3d::DynamicsWorld *_dynamicsWorld;

  std::mutex _createBodyMutex;

  std::unordered_map<Object3D *, PhysicsObject> _objects;

protected:
  PhysicsScene(Scene::Ptr object, rp3d::DynamicsWorld *dynamicsWorld) : _dynamicsWorld(dynamicsWorld) {}

public:
  using Ptr = std::shared_ptr<PhysicsScene>;
  static Ptr make(Scene::Ptr scene, rp3d::DynamicsWorld *dynamicsWorld) {
    return Ptr(new PhysicsScene(scene, dynamicsWorld));
  }

  rp3d::DynamicsWorld *dynamicsWorld() {return _dynamicsWorld;}

  rp3d::RigidBody *getBody(Object3D::Ptr object, bool addShapes=false);
};

}
}

#endif //THREE_PP_MODEL_H
