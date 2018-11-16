//
// Created by byter on 11/9/18.
//

#ifndef THREE_PP_R3DPHYSICSHANDLER_H
#define THREE_PP_R3DPHYSICSHANDLER_H

#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/reactphysics/threereact.h>

class QJsonValueRef;

namespace three {
namespace quick {
namespace r3d {

/**
 * manages physics for a scene
 */
class Physics : public QObject
{
Q_OBJECT
  Q_PROPERTY(int hingeCount READ hingeCount NOTIFY hingesChanged)
  Q_PROPERTY(QStringList hingeNames READ hingeNames NOTIFY hingesChanged)

  friend class PhysicsSceneImpl;

  unsigned hingeIndex = 0;

  std::vector<react3d::HingeData::Ptr> _hinges;

  react3d::PhysicsScene::Ptr _physicsScene;

  int hingeCount () const {return _hinges.size();}
  QStringList hingeNames() const;

  void loadHinges(const QJsonValueRef &json, Object3D::Ptr object);

  void calculateHingeDir(react3d::HingeData &hingeData, const math::Vector3 &hingePoint);

  void createHingePhysics(react3d::HingeData &hingeData);

  void setupDoorHinge(react3d::HingeData &hingeData, const math::Vector3 &hingePoint1World,
                      const math::Vector3 &hingePoint2World);

  void setupPropellerHinge(react3d::HingeData &hingeData, const math::Vector3 &hingePoint1World,
                           const math::Vector3 &hingePoint2World);

  bool checkPhysicsScene();

  Scene *getThreeScene();

  react3d::HingeData::Ptr &nextHinge();

public:
  Physics(QObject *parent=nullptr) : QObject(parent) {}

  const std::vector<react3d::HingeData::Ptr> hinges() const {return _hinges;}

  /**
 * create a door hinge in the middle between 2 marker points which demarcate the
 * hinge axis
 *
 * @param dvar the door
 * @param cvar the part to put the hinge on
 * @param upper upper marker point
 * @param lower lower maraker point
 */
  Q_INVOKABLE void createDoorHinge(QVariant door, QVariant car, QVector3D upper, QVector3D lower);

  /**
 * create a propeller
 *
 * @param propeller the propeller 3D object
 * @param body the plane body
 * @param back the back end of the propeller axis
 * @param lower front the front end of the propeller axis
 */
  Q_INVOKABLE void createPropellerHinge(QVariant propeller, QVariant body, QVector3D back, QVector3D front);

  /**
   * delete a hinge from the current hinge list
   * @param name the name of the hinge (door, rather)
   * @return true if successful
   */
  Q_INVOKABLE bool deleteHinge(const QString &name);

  /**
   * load a physics definition file for the given object
   *
   * @param file the file
   * @param object the object
   * @return whether the operation succeeded
   */
  Q_INVOKABLE bool load(const QString &file, ThreeQObject *object);

  /**
   * fastforward the physics animation
   *
   * @param seconds the number of seconds
   */
  Q_INVOKABLE void fastforward(float seconds);

  /**
   * reset everything, removing all physics data
   */
  Q_INVOKABLE void resetAll();

  /**
   * start the physics timer. Must be called when starting the animation
   */
  Q_INVOKABLE void startTimer();

  /**
   * start the physics timer
   */
  Q_INVOKABLE void stopTimer();

  /**
   * animate the physics world
   */
  Q_INVOKABLE void updateAnimation();

signals:
  void hingesChanged();
};

}
}
}



#endif //THREE_PP_R3DPHYSICSHANDLER_H
