//
// Created by byter on 10/23/18.
//

#include "HingeEditorModelRef.h"

#include <threepp/quick/interact/ObjectPicker.h>
#include <threepp/geometry/Cylinder.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/core/Geometry.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace three {
namespace quick {

namespace tr3 = three::react3d;

HingeEditorModelRef::~HingeEditorModelRef()
{
  _physicsScene = nullptr;
}

void HingeEditorModelRef::startTimer()
{
  hingeIndex = 0;
  for(const auto &hinge : _hinges)
    hinge->elementPhysics->body()->setIsActive(false);

  _physicsScene->timer().start();
  _physicsScene->update();
}

void HingeEditorModelRef::stopTimer()
{
  _physicsScene->timer().stop();
}

void HingeEditorModelRef::resetAll()
{
  _markers.clear();
  _hinges.clear();
  _hingeNames.clear();
  if (_physicsScene) _physicsScene->reset();
}

void HingeEditorModelRef::removeMarkers()
{
  for (auto &marker : _markers) {
    marker->parent()->remove(marker);
  }
  _markers.clear();
}

void HingeEditorModelRef::HingeData::requestUpdate(const math::Quaternion &q)
{
  needsUpdate = true;
}

void HingeEditorModelRef::HingeData::doUpdate()
{
  anchorPhysics->updateFromObject();
  elementPhysics->updateFromObject();

  needsUpdate = false;
}

void HingeEditorModelRef::updateAnimation()
{
  if (_physicsScene) {
    if(hingeIndex > 0)
      _hinges[hingeIndex-1]->elementPhysics->body()->setIsActive(false);
    else
      _hinges.back()->elementPhysics->body()->setIsActive(false);
    HingeData::Ptr &hinge = _hinges[hingeIndex];
    hinge->elementPhysics->body()->setIsActive(true);
    if(++hingeIndex == _hinges.size()) hingeIndex = 0;

    if (hinge->needsUpdate) {
      hinge->doUpdate();
    }

    _physicsScene->update();
  }
}

void HingeEditorModelRef::HingeData::printAngle()
{
  float angle = joint->computeCurrentHingeAngle(anchorPhysics->body()->getTransform().getOrientation(),
                                                elementPhysics->body()->getTransform().getOrientation());
  qDebug() << (clockwise ? "left" : "right") << angle;
}

void HingeEditorModelRef::checkPhysicsScene()
{
  if (!_physicsScene) {
    // Gravity vector in the dynamics world
    rp3d::Vector3 gravity(0, rp3d::decimal(-9.81), 0);

    rp3d::WorldSettings worldSettings;
    worldSettings.worldName = "HingeWorld";
    worldSettings.isSleepingEnabled = false;

    rp3d::DynamicsWorld *dynamicsWorld = new rp3d::DynamicsWorld(gravity, worldSettings);
    _physicsScene = react3d::PhysicsScene::make(_scene->scene(), dynamicsWorld);
  }
}

bool HingeEditorModelRef::saveHingeFile(const QString &file)
{
  QUrl fileUrl(file);
  QFile saveFile(fileUrl.isValid() ? fileUrl.toLocalFile() : file);

  if (!saveFile.open(QIODevice::WriteOnly)) {
    qCritical("Couldn't open file.");
    return false;
  }

  QJsonArray hingeArray;

  for (const auto &hinge : _hinges) {
    QJsonObject hingeObject;

    switch (hinge->hingeType) {
      case HingeType::DOOR:
        hingeObject["type"] = "door";
        break;
      case HingeType::PROPELLER:
        hingeObject["type"] = "propeller";
        break;
    }

    hingeObject["name"] = QString::fromStdString(hinge->element->name());
    hingeObject["anchor"] = QString::fromStdString(hinge->anchor->name());
    hingeObject["element"] = QString::fromStdString(hinge->element->name());

    QJsonObject point1Object;
    point1Object["x"] = hinge->hingePoint1.x();
    point1Object["y"] = hinge->hingePoint1.y();
    point1Object["z"] = hinge->hingePoint1.z();
    hingeObject["point1"] = point1Object;

    QJsonObject point2Object;
    point2Object["x"] = hinge->hingePoint2.x();
    point2Object["y"] = hinge->hingePoint2.y();
    point2Object["z"] = hinge->hingePoint2.z();
    hingeObject["point2"] = point2Object;

    hingeObject["minAngleLimit"] = hinge->minAngleLimit;
    hingeObject["maxAngleLimit"] = hinge->maxAngleLimit;

    hingeArray.push_back(hingeObject);
  }

  QJsonObject docObject;
  docObject["hinges"] = hingeArray;

  QJsonDocument saveDoc(docObject);
  saveFile.write(saveDoc.toJson());

  return true;
}

bool HingeEditorModelRef::loadHingeFile(const QString &file)
{
  QUrl fileUrl(file);
  QFile loadFile(fileUrl.isValid() ? fileUrl.toLocalFile() : file);

  if (!loadFile.open(QIODevice::ReadOnly)) {
    qCritical("Couldn't open file.");
    return false;
  }

  QByteArray saveData = loadFile.readAll();
  QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

  const QJsonValueRef &hingeRef = loadDoc.object()["hinges"];
  if (hingeRef.isNull()) {
    qCritical("Not a hinge definition file.");
    return false;
  }

  resetAll();
  checkPhysicsScene();

  const QJsonArray hingeArray = hingeRef.toArray();

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

    _hinges.emplace_back(HingeData::make(hingeType, _physicsScene));
    HingeData::Ptr &hinge = _hinges.back();

    const QString hingeName = hingeObject["name"].toString();
    _hingeNames.append(hingeName);

    const QString anchorName = hingeObject["anchor"].toString();
    const auto an = anchorName.toStdString();
    hinge->anchor = an == _object->name() ? _object : _object->getChildByName(an);

    const QString elementName = hingeObject["element"].toString();
    const auto en = elementName.toStdString();
    hinge->element = en == _object->name() ? _object : _object->getChildByName(en);

    if (!hinge->element || !hinge->anchor) {

      qCritical() << "Hinge definition does not match model: " << (!hinge->element ? elementName : anchorName)
                  << "not found";
      _hinges.pop_back();
      return false;
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

    createHingePhysics(*hinge, ptw1, ptw2);
  }

  emit hingeNamesChanged();

  return true;
}

bool HingeEditorModelRef::deleteHinge(const QString &name)
{
  std::string qname = name.toStdString();
  for (auto it = _hinges.begin(); it != _hinges.end(); it++) {
    const auto &hinge = *it;
    if (hinge->name == qname) {
      _physicsScene->destroy(hinge->joint);
      _physicsScene->destroy(hinge->element);

      _hingeNames.removeAll(name);
      _hinges.erase(it);

      emit hingeNamesChanged();
      return true;
    }
  }
  return false;
}

void HingeEditorModelRef::createHinge(QVariant dvar, QVariant cvar, QVector3D upper, QVector3D lower)
{
  auto *td = dvar.value<quick::ThreeQObject *>();
  auto *tc = cvar.value<quick::ThreeQObject *>();

  if (!td || !tc) {
    qCritical() << "createHinge: null parameter";
    return;
  }

  //make sure the scene exists
  checkPhysicsScene();

  _hinges.emplace_back(HingeData::make(HingeType::DOOR, _physicsScene));
  HingeData::Ptr &hinge = _hinges.back();

  hinge->anchor = tc->object();
  hinge->element = td->object();

  //save as local points
  math::Vector3 ptw1(upper.x(), upper.y(), upper.z());
  hinge->hingePoint1 = hinge->anchor->worldToLocal(ptw1);
  math::Vector3 ptw2(lower.x(), lower.y(), lower.z());
  hinge->hingePoint2 = hinge->anchor->worldToLocal(ptw2);

  createHingePhysics(*hinge, ptw1, ptw2);

  _hingeNames.append(td->name());
  emit hingeNamesChanged();
}

void HingeEditorModelRef::calculateHingeDir(HingeData &hinge, const math::Vector3 &hingePoint)
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

void HingeEditorModelRef::createHingePhysics(HingeData &hinge,
                                             const math::Vector3 &hingePoint1World, const math::Vector3 &hingePoint2World)
{
  //the anchor is represented by the default shape (bounding box)
  tr3::PhysicsObject *anchorPhysics = _physicsScene->getPhysics(hinge.anchor);
  if (!anchorPhysics) {
    anchorPhysics = _physicsScene->createPhysics(hinge.anchor, rp3d::BodyType::STATIC);
    anchorPhysics->createBoundingBoxShape();
  }
  hinge.anchorPhysics = anchorPhysics;

  //create the moving element and keep a reference
  tr3::PhysicsObject *elementPhysics = _physicsScene->getPhysics(hinge.element);
  if (!elementPhysics) {
    elementPhysics = _physicsScene->createPhysics(hinge.element, rp3d::BodyType::DYNAMIC);
    elementPhysics->createBoundingBoxShape();
  }
  hinge.elementPhysics = elementPhysics;

  //calculate the hinge axis
  const auto ax = (hingePoint1World - hingePoint2World).normalized();
  rp3d::Vector3 hingeAxisWorld(ax.x(), ax.y(), ax.z());

  //place the hinge in the middle between the hinge points
  const auto pt = (hingePoint1World + hingePoint2World) * 0.5;
  rp3d::Vector3 hingePointWorld(pt.x(), pt.y(), pt.z());

  calculateHingeDir(hinge, pt);

  rp3d::HingeJointInfo jointInfo = rp3d::HingeJointInfo(elementPhysics->body(), anchorPhysics->body(), hingePointWorld,
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

void HingeEditorModelRef::setMarker(three::quick::ObjectPicker *picker)
{
  float radius = picker->scaleSize() ? picker->scaleSize() / 100 * 2 : 5;
  geometry::Cylinder::Ptr cylinder = geometry::Cylinder::make(radius, radius, radius);

  three::MeshBasicMaterial::Ptr mat = three::MeshBasicMaterial::make();
  mat->color = Color(ColorName::red);
  auto marker = DynamicMesh::make(cylinder);
  marker->setMaterial(mat);
  marker->setName("HingeMarker");

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
  _markers.push_back(marker);
}

}
}