//
// Created by byter on 11/9/18.
//

#include "R3dPhysics.hxx"
#include "Scene.h"
#include <threepp/quick/loader/FileSystemLoader.h>
#include <threepp/quick/loader/QtResourceLoader.h>
#include <threepp/util/impl/utils.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

namespace three {
namespace quick {
namespace r3d {

using namespace three::react3d;

class PhysicsSceneImpl : public PhysicsScene
{
  Physics * const _physics;
  HingeData::Ptr currentHinge;

protected:
  void preUpdate() override {
    currentHinge = _physics->nextHinge();
    if (currentHinge->needsUpdate)  currentHinge->doUpdate();
  }

  void postUpdate() override {
    currentHinge->needsUpdate = false;
  }

  PhysicsSceneImpl(Physics *physics, three::Scene::Ptr scene, const rp3d::Vector3 &gravity,
     const rp3d::WorldSettings &worldSettings)
     : PhysicsScene(scene, gravity, worldSettings), _physics(physics)
  {}

public:
  using Ptr = std::shared_ptr<PhysicsSceneImpl>;

  static Ptr make(Physics *physics, three::Scene::Ptr scene, const rp3d::Vector3 &gravity,
                  const rp3d::WorldSettings &worldSettings)
  {
    return Ptr(new PhysicsSceneImpl(physics, scene, gravity, worldSettings));
  }
};

Scene *Physics::getThreeScene()
{
  Scene *scene = dynamic_cast<Scene *>(parent());
  if(!scene) {
    qCritical() << "unable to find parent Scene";
    return nullptr;
  }
  return scene;
}

HingeData::Ptr &Physics::nextHinge()
{
  if(hingeIndex > 0)
    _hinges[hingeIndex-1]->elementPhysics->body()->setIsActive(false);
  else
    _hinges.back()->elementPhysics->body()->setIsActive(false);
  HingeData::Ptr &hinge = _hinges[hingeIndex];
  hinge->elementPhysics->body()->setIsActive(true);
  if(++hingeIndex == _hinges.size()) hingeIndex = 0;

  return hinge;
}

void Physics::updateAnimation()
{
  if (_physicsScene) {

    _physicsScene->timedUpdate();
  }
}

void Physics::fastforward(float seconds)
{
  hingeIndex = 0;
  for(const auto &hinge : _hinges)
    hinge->elementPhysics->body()->setIsActive(false);

  for(unsigned i=0; i<_hinges.size(); i++) {

    _physicsScene->update(seconds);
  }
}

void Physics::startTimer()
{
  hingeIndex = 0;
  for(const auto &hinge : _hinges)
    hinge->elementPhysics->body()->setIsActive(false);

  _physicsScene->timer().start();
}

void Physics::stopTimer()
{
  _physicsScene->timer().stop();
}

bool Physics::checkPhysicsScene()
{
  Scene * scene = getThreeScene();
  if(!scene || !scene->scene()) return false;

  if (!_physicsScene) {
    // Gravity vector in the dynamics world
    rp3d::Vector3 gravity(0, rp3d::decimal(-9.81), 0);

    rp3d::WorldSettings worldSettings;
    worldSettings.worldName = "R3dPhysics";
    worldSettings.isSleepingEnabled = false;

    _physicsScene = PhysicsSceneImpl::make(this, scene->scene(), gravity, worldSettings);
  }
  return true;
}

QStringList Physics::hingeNames() const
{
  QStringList names;
  for(const auto &hinge : _hinges) names.append(QString::fromStdString(hinge->name));
  return names;
}

bool Physics::deleteHinge(const QString &name)
{
  std::string qname = name.toStdString();
  for (auto it = _hinges.begin(); it != _hinges.end(); it++) {
    const auto &hinge = *it;
    if (hinge->name == qname) {
      _physicsScene->destroy(hinge->joint);
      _physicsScene->destroy(hinge->element);

      _hinges.erase(it);

      emit hingesChanged();
      return true;
    }
  }
  return false;
}

void Physics::loadHinges(const QJsonValueRef &json, Object3D::Ptr object)
{
  resetAll();
  checkPhysicsScene();

  const QJsonArray hingeArray = json.toArray();

  for (int i = 0; i < hingeArray.size(); i++) {
    QJsonObject hingeObject = hingeArray[i].toObject();

    HingeType hingeType;
    const auto ht = hingeObject["type"].toString();
    if (ht == "door")
      hingeType = HingeType::DOOR;
    else if (ht == "propeller")
      hingeType = HingeType::PROPELLER;
    else
      throw std::invalid_argument("unknown hinge type");

    _hinges.emplace_back(HingeData::make(hingeType));
    HingeData::Ptr &hinge = _hinges.back();

    hinge->name = hingeObject["name"].toString().toStdString();

    const QString anchorName = hingeObject["anchor"].toString();
    const auto an = anchorName.toStdString();
    hinge->anchor = an == object->name() ? object : object->getChildByName(an);

    const QString elementName = hingeObject["element"].toString();
    const auto en = elementName.toStdString();
    hinge->element = en == object->name() ? object : object->getChildByName(en);

    if (!hinge->element || !hinge->anchor) {

      qCritical() << "Hinge definition does not match model: " << (!hinge->element ? elementName : anchorName)
                  << "not found";
      _hinges.pop_back();
      return;
    }

    const QJsonObject pointObject = hingeObject["point1"].toObject();
    hinge->hingePoint1.set(pointObject["x"].toDouble(),
                           pointObject["y"].toDouble(),
                           pointObject["z"].toDouble());

    const QJsonObject axisObject = hingeObject["point2"].toObject();
    hinge->hingePoint2.set(axisObject["x"].toDouble(),
                           axisObject["y"].toDouble(),
                           axisObject["z"].toDouble());

    hinge->minAngleLimit = hingeObject["minAngleLimit"].toDouble();
    hinge->maxAngleLimit = hingeObject["maxAngleLimit"].toDouble();

    const auto ptw1 = hinge->anchor->localToWorld(hinge->hingePoint1);
    const auto ptw2 = hinge->anchor->localToWorld(hinge->hingePoint2);

    createHingePhysics(*hinge);

    switch(hingeType) {
      case HingeType::DOOR:
        setupDoorHinge(*hinge, ptw1, ptw2);
        break;
      case HingeType::PROPELLER:
        setupPropellerHinge(*hinge, ptw1, ptw2);
        break;
    }
  }
  emit hingesChanged();
}

bool Physics::load(const QString &file, ThreeQObject *object)
{
  QUrl fileUrl(file);
  QFile loadFile(fileUrl.isLocalFile() ? fileUrl.toLocalFile() : file);

  if (!loadFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Couldn't open physics file " << file;
    return false;
  }

  QByteArray saveData = loadFile.readAll();
  QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

  const QJsonValueRef &hingeRef = loadDoc.object()["hinges"];
  if (!hingeRef.isNull()) {
    loadHinges(hingeRef, object->object());
    return true;
  }
  return false;
}

void Physics::createPropellerHinge(QVariant propeller, QVariant body, QVector3D back, QVector3D front)
{
  auto *prop = propeller.value<quick::ThreeQObject *>();
  auto *bdy = body.value<quick::ThreeQObject *>();

  if (!prop || !bdy) {
    qCritical() << "createPropeller: null parameter";
    return;
  }

  //make sure the scene exists
  if(!checkPhysicsScene()) return;

  _hinges.emplace_back(HingeData::make(HingeType::PROPELLER));
  HingeData::Ptr &hinge = _hinges.back();

  hinge->anchor = bdy->object();
  hinge->element = prop->object();

  //save as local points
  math::Vector3 ptw1(back.x(), back.y(), back.z());
  hinge->hingePoint1 = hinge->anchor->worldToLocal(ptw1);
  math::Vector3 ptw2(front.x(), front.y(), front.z());
  hinge->hingePoint2 = hinge->anchor->worldToLocal(ptw2);

  createHingePhysics(*hinge);

  setupPropellerHinge(*hinge, ptw1, ptw2);

  emit hingesChanged();
}

void Physics::createDoorHinge(QVariant dvar, QVariant cvar, QVector3D upper, QVector3D lower)
{
  auto *td = dvar.value<quick::ThreeQObject *>();
  auto *tc = cvar.value<quick::ThreeQObject *>();

  if (!td || !tc) {
    qCritical() << "createHinge: null parameter";
    return;
  }

  //make sure the scene exists
  if(!checkPhysicsScene()) return;

  _hinges.emplace_back(HingeData::make(HingeType::DOOR));
  HingeData::Ptr &hinge = _hinges.back();

  hinge->anchor = tc->object();
  hinge->element = td->object();

  //save as local points
  math::Vector3 ptw1(upper.x(), upper.y(), upper.z());
  hinge->hingePoint1 = hinge->anchor->worldToLocal(ptw1);
  math::Vector3 ptw2(lower.x(), lower.y(), lower.z());
  hinge->hingePoint2 = hinge->anchor->worldToLocal(ptw2);

  createHingePhysics(*hinge);

  setupDoorHinge(*hinge, ptw1, ptw2);

  emit hingesChanged();
}

void Physics::calculateHingeDir(HingeData &hinge, const math::Vector3 &hingePoint)
{
  const auto hingePtLocal = hinge.anchor->worldToLocal(hingePoint);
  math::Line3 line(hinge.anchorPhysics->boxPosition(), hingePtLocal);
  const auto delta = line.delta().normalized();

  float dx = delta.x() * delta.y();

  hinge.hingeDir = dx < 0.1f && dx > -0.1f ? HingeDir::UP : (dx < 0.0f ? HingeDir::LEFT : HingeDir::RIGHT);
  switch(hinge.hingeDir) {
    case HingeDir::UP:
      qDebug() << "UP" << dx;
      break;
    case HingeDir::LEFT:
      qDebug() << "LEFT" << dx;
      break;
    case HingeDir::RIGHT:
      qDebug() << "RIGHT" << dx;
      break;
  }
}

void Physics::createHingePhysics(HingeData &hinge)
{
  //the anchor is represented by the default shape (bounding box)
  PhysicsObject *anchorPhysics = _physicsScene->getPhysics(hinge.anchor);
  if (!anchorPhysics) {
    anchorPhysics = _physicsScene->createPhysics(hinge.anchor, rp3d::BodyType::STATIC);
    anchorPhysics->createBoundingBoxShape();
  }
  hinge.anchorPhysics = anchorPhysics;

  //create the moving element and keep a reference
  PhysicsObject *elementPhysics = _physicsScene->getPhysics(hinge.element);
  if (!elementPhysics) {
    elementPhysics = _physicsScene->createPhysics(hinge.element, rp3d::BodyType::DYNAMIC);
    elementPhysics->createBoundingBoxShape();
  }
  hinge.elementPhysics = elementPhysics;
}

void Physics::setupDoorHinge(HingeData &hinge, const math::Vector3 &hingePoint1World, const math::Vector3 &hingePoint2World)
{
  //calculate the hinge axis
  const auto ax = (hingePoint1World - hingePoint2World).normalized();
  rp3d::Vector3 hingeAxisWorld(ax.x(), ax.y(), ax.z());

  //place the hinge in the middle between the hinge points
  const auto pt = (hingePoint1World + hingePoint2World) * 0.5;
  rp3d::Vector3 hingePointWorld(pt.x(), pt.y(), pt.z());

  calculateHingeDir(hinge, pt);

  rp3d::HingeJointInfo jointInfo = rp3d::HingeJointInfo(hinge.elementPhysics->body(),
                                                        hinge.anchorPhysics->body(),
                                                        hingePointWorld,
                                                        hingeAxisWorld);

  jointInfo.isLimitEnabled = true;
  jointInfo.isMotorEnabled = true;
  jointInfo.maxMotorTorque = rp3d::decimal(20.0);
  jointInfo.isCollisionEnabled = false;

  switch(hinge.hingeDir) {
    case HingeDir::UP:
      jointInfo.motorSpeed = rp3d::decimal(0.05) * M_PI;
      jointInfo.minAngleLimit = -M_PI_2 * 0.7;
      jointInfo.maxAngleLimit = 0;
      break;
    case HingeDir::LEFT:
      jointInfo.motorSpeed = - rp3d::decimal(0.05) * M_PI;
      jointInfo.minAngleLimit = 0;
      jointInfo.maxAngleLimit = M_PI_2 * 0.8;
      break;
    case HingeDir::RIGHT:
      jointInfo.motorSpeed = rp3d::decimal(0.05) * M_PI;
      jointInfo.minAngleLimit = -M_PI_2 * 0.8;
      jointInfo.maxAngleLimit = 0;
      break;
  }
  hinge.anchor->quaternion().onChange.connect(&hinge, &HingeData::requestUpdate);

  // Create the joint in the dynamics world
  hinge.joint = _physicsScene->createHingeJoint(jointInfo);
}

void Physics::setupPropellerHinge(HingeData &hinge, const math::Vector3 &hingePoint1World, const math::Vector3 &hingePoint2World)
{
  //calculate the hinge axis
  const auto ax = (hingePoint1World - hingePoint2World).normalized();
  rp3d::Vector3 hingeAxisWorld(ax.x(), ax.y(), ax.z());

  rp3d::Vector3 hingePointWorld(hingePoint2World.x(), hingePoint2World.y(), hingePoint2World.z());

  //const auto &bbox = hinge.anchorPhysics->getBoundingBox();

  rp3d::HingeJointInfo jointInfo = rp3d::HingeJointInfo(hinge.elementPhysics->body(),
                                                        hinge.anchorPhysics->body(),
                                                        hingePointWorld,
                                                        hingeAxisWorld);

  jointInfo.isLimitEnabled = false;
  jointInfo.isMotorEnabled = true;
  jointInfo.maxMotorTorque = rp3d::decimal(60.0);
  jointInfo.isCollisionEnabled = false;
  jointInfo.motorSpeed = - 4 * M_PI;

  hinge.anchor->quaternion().onChange.connect(&hinge, &HingeData::requestUpdate);

  // Create the joint in the dynamics world
  hinge.joint = _physicsScene->createHingeJoint(jointInfo);
}

void Physics::resetAll()
{
  _hinges.clear();
  if (_physicsScene) _physicsScene->reset();
  emit hingesChanged();
}


}
}
}
