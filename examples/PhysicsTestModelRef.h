//
// Created by byter on 10/23/18.
//

#ifndef THREE_PP_TESTMODELREF_H
#define THREE_PP_TESTMODELREF_H

#include <threepp/quick/objects/ModelRef.h>
#include <threepp/reactphysics/threereact.h>

namespace three {
namespace quick {

/**
 * subclasses ModelRef to demonstrate integration with reactphysics3d
 */
class PhysicsTestModelRef : public quick::ModelRef
{
Q_OBJECT

  react3d::PhysicsScene::Ptr _physicsScene;
  rp3d::HingeJoint * _doorJoint;
  rp3d::RigidBody *_doorBody;
  float _force = 1.0f;

public:
  PhysicsTestModelRef(QObject *parent = nullptr)  : ModelRef(parent) {}
  ~PhysicsTestModelRef() override;

/**
 * create a door hinge in the middle between 2 marker points which also demarcate the
 * hinge axis
 *
 * @param dvar the door
 * @param cvar the part to put the hinge on
 * @param upper upper marker point
 * @param lower lower maraker point
 */
  Q_INVOKABLE void createHinge(QVariant door, QVariant car, QVector3D upper, QVector3D lower);

/*
 * set a marker on the model at the last picked position
 */
  Q_INVOKABLE void setMarker(three::quick::ObjectPicker *picker);

  /**
   * start the physics timer
   */
  Q_INVOKABLE void start();

  /**
   * start the physics timer
   */
  Q_INVOKABLE void stop();

  /**
   * update the physics
   */
  Q_INVOKABLE void update();
};

}
}


#endif //THREE_PP_TESTMODELREF_H
