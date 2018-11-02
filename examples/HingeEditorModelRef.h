//
// Created by byter on 10/23/18.
//

#ifndef THREE_PP_TESTMODELREF_H
#define THREE_PP_TESTMODELREF_H

//#define IS_LOGGING_ACTIVE
#include <threepp/quick/objects/ModelRef.h>
#include <threepp/reactphysics/threereact.h>
#include <QStringListModel>

namespace three {
namespace quick {

/**
 * subclasses ModelRef to provide support functions for hinge definition based on the
 * physics integration with reactphysics3d
 */
class HingeEditorModelRef : public quick::ModelRef
{
Q_OBJECT

  Q_PROPERTY(QStringList hingeNames READ hingeNames NOTIFY hingeNamesChanged)

  enum HingeType {DOOR, PROPELLER};

  struct HingeData
  {
    HingeType hingeType;
    std::string name;
    three::Object3D::Ptr car;
    three::Object3D::Ptr door;
    rp3d::Vector3 hingePoint;
    rp3d::Vector3 hingeAxis;
    float minAngleLimit;
    float maxAngleLimit;

    rp3d::HingeJoint *joint = nullptr;
    rp3d::RigidBody *elementBody = nullptr;
    rp3d::RigidBody *anchorBody = nullptr;

    HingeData(HingeType hingeType) : hingeType(hingeType) {}
  };

  std::vector<HingeData> _hinges;

  std::vector<DynamicMesh::Ptr> _markers;

  QStringList _hingeNames;

  size_t _currentHinge = 0;

  react3d::PhysicsScene::Ptr _physicsScene;
  float _force = 1.0f;

  void createHingePhysics(HingeData &hingeData);

  void checkPhysicsScene();

  const QStringList &hingeNames() {return _hingeNames;}

public:
  HingeEditorModelRef(QObject *parent = nullptr)  : ModelRef(parent) {}
  ~HingeEditorModelRef() override;

  /**
   * save the current hinge definition to a file
   *
   * @param file
   */
  Q_INVOKABLE bool saveHingeFile(const QString &file);

  /**
   * load a hinge definition from a file
   *
   * @param file
   */
  Q_INVOKABLE bool loadHingeFile(const QString &file);

  /**
   * delete a hinge from the current hinge list
   * @param name the name of the hinge (door, rather)
   * @return true if successful
   */
  Q_INVOKABLE bool deleteHinge(const QString &name);

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

  /**
   * set a marker on the model at the last picked position
   */
  Q_INVOKABLE void setMarker(three::quick::ObjectPicker *picker);

  /**
   * start the physics timer. Must be called when starting the animation
   */
  Q_INVOKABLE void startTimer();

  /**
   * start the physics timer
   */
  Q_INVOKABLE void stopTimer();

  /**
   * animate the hinges
   */
  Q_INVOKABLE void updateAnimation();

  /**
   * reset everything, removing all physics data
   */
  Q_INVOKABLE void resetAll();

  /**
   * remove all current hinge markers
   */
  Q_INVOKABLE void removeMarkers();

signals:
  void hingeNamesChanged();
};

}
}


#endif //THREE_PP_TESTMODELREF_H
