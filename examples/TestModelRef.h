//
// Created by byter on 10/23/18.
//

#ifndef THREE_PP_TESTMODELREF_H
#define THREE_PP_TESTMODELREF_H

#include <threepp/quick/objects/ModelRef.h>
#include <threepp/reactphysics/threereact.h>

namespace three {
namespace quick {

class TestModelRef : public quick::ModelRef
{
Q_OBJECT

  react3d::PhysicsScene::Ptr _physicsScene;
  rp3d::HingeJoint * _doorJoint;

public:

  Q_INVOKABLE void createHinges(quick::ThreeQObject *car, quick::ThreeQObject *door, QVector3D upper, QVector3D lower);

  Q_INVOKABLE void testMarker(three::quick::ObjectPicker *picker);
};

}
}


#endif //THREE_PP_TESTMODELREF_H
