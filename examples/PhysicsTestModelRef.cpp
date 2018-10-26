//
// Created by byter on 10/23/18.
//

#include "PhysicsTestModelRef.h"

#include <threepp/quick/interact/ObjectPicker.h>
#include <threepp/geometry/Cylinder.h>
#include <threepp/material/MeshBasicMaterial.h>

namespace three {
namespace quick {

void PhysicsTestModelRef::createHinge(QVariant cvar, QVariant dvar, QVector3D upper, QVector3D lower)
{
  quick::ThreeQObject *car = cvar.value<quick::ThreeQObject *>();
  quick::ThreeQObject *door = dvar.value<quick::ThreeQObject *>();

  if(!car || !door) {
    qCritical() << "createHinge: null parameter";
    return;
  }

  if(!_physicsScene) {
    // Gravity vector in the dynamics world
    rp3d::Vector3 gravity(0, rp3d::decimal(-9.81), 0);

    rp3d::WorldSettings worldSettings;
    worldSettings.worldName = "ThreeDynamics";

    rp3d::DynamicsWorld *dynamicsWorld = new rp3d::DynamicsWorld(gravity, worldSettings);
    _physicsScene = react3d::PhysicsScene::make(_scene->scene(), dynamicsWorld);
  }
  rp3d::RigidBody *body1 = _physicsScene->getBody(car->object());
  rp3d::RigidBody *body2 = _physicsScene->getBody(door->object());

  auto hingePoint = (upper + lower) * 0.5;
  auto hingeAxis = (upper - lower).normalized();

  rp3d::HingeJointInfo jointInfo(body1, body2,
                                 rp3d::Vector3(hingePoint.x(), hingePoint.y(), hingePoint.z()),
                                 rp3d::Vector3(hingeAxis.x(), hingeAxis.y(), hingeAxis.z()));

  jointInfo.isMotorEnabled = false;
  jointInfo.motorSpeed = - rp3d::decimal(0.5) * M_PI;
  jointInfo.maxMotorTorque = rp3d::decimal(60.0);
  jointInfo.isCollisionEnabled = false;

  // Create the joint in the dynamics world
  _doorJoint = dynamic_cast<rp3d::HingeJoint*>(_physicsScene->dynamicsWorld()->createJoint(jointInfo));
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