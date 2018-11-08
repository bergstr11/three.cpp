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

  enum class HingeType {DOOR, PROPELLER};
  enum class HingeDir {LEFT, RIGHT, UP};

  struct HingeData
  {
    HingeType hingeType;
    std::string name;
    Object3D::Ptr anchor;
    Object3D::Ptr element;
    math::Vector3 hingePoint1;
    math::Vector3 hingePoint2;
    bool clockwise;
    float minAngleLimit;
    float maxAngleLimit;

    HingeDir hingeDir;
    rp3d::HingeJoint *joint = nullptr;
    react3d::PhysicsObject *elementPhysics = nullptr;
    react3d::PhysicsObject *anchorPhysics = nullptr;

    bool needsUpdate = false;
    react3d::PhysicsScene::Ptr _scene;

    HingeData(HingeType hingeType, react3d::PhysicsScene::Ptr scene)
       : hingeType(hingeType), _scene(scene)
    {}

    void doUpdate();
    void requestUpdate(const math::Quaternion &q);
    void printAngle();

    using Ptr = std::shared_ptr<HingeData>;
    static Ptr make(HingeType hingeType, react3d::PhysicsScene::Ptr scene) {
      return Ptr(new HingeData(hingeType, scene));
    }
  };

  unsigned hingeIndex = 0;

  std::vector<HingeData::Ptr> _hinges;

  std::vector<DynamicMesh::Ptr> _markers;

  QStringList _hingeNames;

  react3d::PhysicsScene::Ptr _physicsScene;

  void calculateHingeDir(HingeData &hingeData, const math::Vector3 &hingePoint);

  void createHingePhysics(HingeData &hingeData, const math::Vector3 &hingePoint1World, const math::Vector3 &hingePoint2World);

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
