//
// Created by byter on 11/9/18.
//

#ifndef THREE_PP_R3DPHYSICSHANDLER_H
#define THREE_PP_R3DPHYSICSHANDLER_H

#include <threepp/quick/objects/ThreeQObject.h>
#include <QElapsedTimer>

class QJsonValueRef;

namespace three {
namespace quick {

struct Hinge
{
  enum class Direction {CLOCKWISE, COUNTERCLOCKWISE};

  std::string name;

  float angleLimit = 0;
  float rotatedAngle = 0;

  long upm = 60;

  virtual void rotate(float angle) const = 0;

  using Ptr = std::shared_ptr<Hinge>;
};

/**
 * manages dynamics for a scene
 */
class Dynamics : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(int hingeCount READ hingeCount NOTIFY hingesChanged)
  Q_PROPERTY(QStringList hingeNames READ hingeNames NOTIFY hingesChanged)

  std::vector<Hinge::Ptr> _hinges;

  QElapsedTimer _timer;

  int hingeCount () const {return _hinges.size();}
  QStringList hingeNames() const;

  void loadHinges(const QJsonValueRef &json, Object3D::Ptr object);

public:
  Dynamics(QObject *parent=nullptr) : ThreeQObjectRoot(parent) {}

  const std::vector<Hinge::Ptr> &hinges() {return _hinges;}

/**
 * create a door hinge in the middle between 2 marker points which demarcate the
 * hinge axis
 *
 * @param doorthe door
 * @param body the part to put the hinge on
 * @param upper upper marker point
 * @param lower lower maraker point
 */
  Q_INVOKABLE void createDoorHinge(QVariant door, QVariant body, QVector3D upper, QVector3D lower);

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
   * create hinges for tow wheels mounted to the same axis
   *
   * @param left the left wheel
   * @param right the right wheel
   * @param leftMid the midpoint of the left wheel
   * @param rightMid the midpoint of the right wheel
   */
  Q_INVOKABLE void createWheelHinge(QVariant leftWheel, QVariant rightWheel, QVector3D front);

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
   * save a physics definition file
   *
   * @param file the file
   * @return whether the operation succeeded
   */
  Q_INVOKABLE bool save(const QString &file);

  /**
   * fastforward the animation
   *
   * @param seconds the number of seconds
   */
  Q_INVOKABLE void fastforward(float seconds);

  /**
   * reset everything, removing all physics data
   */
  Q_INVOKABLE void resetAll();

  /**
   * animate the physics world
   */
  Q_INVOKABLE void update();

signals:
  void hingesChanged();
};

}
}

#endif //THREE_PP_R3DPHYSICSHANDLER_H
