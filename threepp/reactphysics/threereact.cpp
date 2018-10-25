//
// Created by byter on 10/24/18.
//

#include "threereact.h"
#include <threepp/geometry/Box.h>

namespace three {
namespace react3d {

rp3d::RigidBody *PhysicsScene::getBody(Object3D::Ptr object, bool addShapes)
{
  auto found = _objects.find(object.get());
  if(found != _objects.end()) return found->second.body;

  {
    std::lock_guard<std::mutex> lck(_createBodyMutex);

    auto found = _objects.find(object.get());
    if(found != _objects.end()) return found->second.body;

    rp3d::Transform transform = rp3d::Transform::identity();
    rp3d::RigidBody *body = _dynamicsWorld->createRigidBody(transform);
    _objects.emplace(object.get(), PhysicsObject(body, transform));

    if(addShapes) {
      if(geometry::BoxParams *box = object->geometry()->typer) {
        //body->addCollisionShape();
      }
      else if(LinearGeometry *geom = object->geometry()->typer) {

      }
      else if(BufferGeometry *geom = object->geometry()->typer) {

      }
    }

    return body;
  }
}

}
}