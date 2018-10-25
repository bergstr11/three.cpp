//
// Created by byter on 10/23/18.
//

#include "TestModelRef.h"

#include <threepp/quick/interact/ObjectPicker.h>
#include <threepp/geometry/Box.h>
#include <threepp/material/MeshBasicMaterial.h>

namespace three {
namespace quick {

void TestModelRef::createHinges(quick::ThreeQObject *car, quick::ThreeQObject *door, QVector3D upper, QVector3D lower)
{
  if(!_physicsScene) {
    // Gravity vector in the dynamics world
    rp3d::Vector3 gravity(0, rp3d::decimal(-9.81), 0);

    rp3d::WorldSettings worldSettings;
    worldSettings.worldName = "ThreeDynamics";

    rp3d::DynamicsWorld *dynamicsWorld = new rp3d::DynamicsWorld(gravity, worldSettings);
    _physicsScene = react3d::PhysicsScene::make(_scene->scene(), dynamicsWorld);
  }
  rp3d::RigidBody *body1 = _physicsScene->getBody(car->object());
  rp3d::RigidBody *body2 = _physicsScene->getBody(door->object());;
  const rp3d::Vector3& body1Position = body1->getTransform().getPosition();
  const rp3d::Vector3& body2Position = body2->getTransform().getPosition();
  const rp3d::Vector3 anchorPointWorldSpace = 0.5 * (body2Position + body1Position);
  const rp3d::Vector3 hingeAxisWorldSpace(0, 1, 0);
  rp3d::HingeJointInfo jointInfo(body1, body2, anchorPointWorldSpace, hingeAxisWorldSpace);
  jointInfo.isMotorEnabled = true;
  jointInfo.motorSpeed = - rp3d::decimal(0.5) * M_PI;
  jointInfo.maxMotorTorque = rp3d::decimal(60.0);
  jointInfo.isCollisionEnabled = false;

  // Create the joint in the dynamics world
  _doorJoint = dynamic_cast<rp3d::HingeJoint*>(_physicsScene->dynamicsWorld()->createJoint(jointInfo));
}

/*
 * ThreeD {
        ObjectPicker
        {
            id: picker
            camera: scene.camera
            enabled: true

            rays: CircularRays {radius: 5; segments: 24}

            onObjectsClicked: {
                modelref.testMarker(picker)
                threeD.update()
            }
        }

      ModelRef onObjectChanged: picker.objects = [modelref.object]
 *
 */
void TestModelRef::testMarker(three::quick::ObjectPicker *picker)
{
  float radius = picker->scaleSize() ? picker->scaleSize() / 100 * 2 : 5;
  geometry::Box::Ptr box = geometry::Box::make(radius, radius, radius / 5);

  three::MeshBasicMaterial::Ptr mat = three::MeshBasicMaterial::make();
  mat->color = Color(ColorName::green);
  mat->wireframe = true;
  auto mesh = DynamicMesh::make(box);
  mesh->setMaterial(mat);

  const math::Vector3 &snorm = picker->getRays().surfaceNormal();

  math::Vector3 up(0, 0, 1);
  mesh->quaternion().setFromUnitVectors(up, snorm);

  mesh->position() = picker->pickedObject()->worldToLocal(picker->getRays().surfacePosition());
  mesh->setName("TestMarker");

  picker->pickedObject()->add(mesh);
}

}
}