//
// Created by byter on 10/23/18.
//

#include "PhysicsTestModelRef.h"

#include <threepp/quick/interact/ObjectPicker.h>
#include <threepp/geometry/Cylinder.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/core/Geometry.h>

namespace three {
namespace quick {

namespace tr3 = three::react3d;

PhysicsTestModelRef::~PhysicsTestModelRef()
{
  _physicsScene = nullptr;
  _doorJoint = nullptr;
}

void PhysicsTestModelRef::start()
{
  _physicsScene->timer().start();
  _physicsScene->update();

  _force = -2.0f;
}

void PhysicsTestModelRef::stop()
{
  _physicsScene->timer().stop();
}

void PhysicsTestModelRef::update()
{
  if(_physicsScene) {

    _doorBody->applyForceToCenterOfMass(rp3d::Vector3(0, _force, 0));
    _physicsScene->update();
  }
}

void PhysicsTestModelRef::createHinge(QVariant dvar, QVariant cvar, QVector3D upper, QVector3D lower)
{
  quick::ThreeQObject *door = dvar.value<quick::ThreeQObject *>();
  quick::ThreeQObject *car = cvar.value<quick::ThreeQObject *>();

  if(!car || !door) {
    qCritical() << "createHinge: null parameter";
    return;
  }

  if(!_physicsScene) {
    // Gravity vector in the dynamics world
    rp3d::Vector3 gravity(0, rp3d::decimal(-9.81), 0);

    rp3d::WorldSettings worldSettings;
    worldSettings.worldName = "HingeWorld";

    rp3d::DynamicsWorld *dynamicsWorld = new rp3d::DynamicsWorld(gravity, worldSettings);
    _physicsScene = react3d::PhysicsScene::make(_scene->scene(), dynamicsWorld);
  }

  //a simple box along the hinge axis will do for the car
  float distance = upper.distanceToPoint(lower);
  rp3d::BoxShape *carBox = new rp3d::BoxShape(rp3d::Vector3(10, distance, 10));

  //the door is represented by its bounding box
  const three::math::Vector3 &bb = door->object()->computeBoundingBox().getSize();
  rp3d::BoxShape *doorBox = new rp3d::BoxShape(rp3d::Vector3(bb.x(), bb.y(), bb.z()));

  //car is static, we only want the door to move
  tr3::PhysicsObject &carPhysics = _physicsScene->getPhysics(car->object());
  carPhysics.body()->addCollisionShape(carBox, rp3d::Transform::identity(), 1.0f);
  carPhysics.body()->setType(rp3d::BodyType::STATIC);

  //create the door and keep a reference
  tr3::PhysicsObject &doorPhysics = _physicsScene->getPhysics(door->object());
  doorPhysics.body()->addCollisionShape(doorBox, rp3d::Transform::identity(), 1.0f);
  _doorBody = doorPhysics.body();

  //place the hinge in the middle between the hinge points
  auto hingePoint = (upper + lower) * 0.5;
  auto hingeAxis = (lower - upper).normalized();

  rp3d::HingeJointInfo jointInfo(carPhysics.body(), doorPhysics.body(),
                                 rp3d::Vector3(hingePoint.x(), hingePoint.y(), hingePoint.z()),
                                 rp3d::Vector3(hingeAxis.x(), hingeAxis.y(), hingeAxis.z()));

  jointInfo.isLimitEnabled = true;
  jointInfo.minAngleLimit = -M_PI_2;
  jointInfo.maxAngleLimit = 0;
  jointInfo.isCollisionEnabled = false;

  // Create the joint in the dynamics world
  _doorJoint = _physicsScene->createHingeJoint(jointInfo);
}

void PhysicsTestModelRef::setMarker(three::quick::ObjectPicker *picker)
{
  float radius = picker->scaleSize() ? picker->scaleSize() / 100 * 2 : 5;
  geometry::Cylinder::Ptr cylinder = geometry::Cylinder::make(radius, radius, radius);

  three::MeshBasicMaterial::Ptr mat = three::MeshBasicMaterial::make();
  mat->color = Color(ColorName::red);
  auto marker = DynamicMesh::make(cylinder);
  marker->setMaterial(mat);
  marker->setName("TestMarker");

  float distance = picker->camera()->camera()->getWorldPosition().distanceTo(picker->pickedObject()->getWorldPosition());

  marker->scale() = distance / 800.0f;

  //adjust orientation
  math::Vector3 up {0, 1, 0};
  const math::Vector3 &snorm = picker->getRays().surfaceNormal();
  marker->quaternion().setFromUnitVectors(up, snorm);

  math::Vector3 flip {0, 1, 0};
  marker->rotateOnAxis(flip, M_PI_2);

  //put on target
  marker->position() = picker->pickedObject()->worldToLocal(picker->getRays().surfacePosition());

  picker->pickedObject()->add(marker);
}

}
}