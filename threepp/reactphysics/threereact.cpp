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