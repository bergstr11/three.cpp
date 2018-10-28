//
// Created by byter on 10/23/18.
//

#ifndef THREE_PP_TESTMODELREF_H
#define THREE_PP_TESTMODELREF_H

#include <threepp/quick/objects/ModelRef.h>
#include <threepp/reactphysics/threereact.h>

namespace three {
namespace quick {

class PhysicsTestModelRef : public quick::ModelRef
{
Q_OBJECT

  react3d::PhysicsScene::Ptr _physicsScene;
  rp3d::HingeJoint * _doorJoint;
  rp3d::RigidBody *_doorBody;

public:
  PhysicsTestModelRef(QObject *parent = nullptr)  : ModelRef(parent) {}
  ~PhysicsTestModelRef() override;

  Q_INVOKABLE void createHinge(QVariant door, QVariant car, QVector3D upper, QVector3D lower);

  Q_INVOKABLE void setMarker(three::quick::ObjectPicker *picker);

  Q_INVOKABLE void start();
  Q_INVOKABLE void stop();
  Q_INVOKABLE void update();
};

}
}


#endif //THREE_PP_TESTMODELREF_H
