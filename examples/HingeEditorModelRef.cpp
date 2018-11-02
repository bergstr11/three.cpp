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
  _physicsScene->timer().start();
  _physicsScene->update();

  _force = -2.0f;
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
  if(_physicsScene) _physicsScene->reset();
}

void HingeEditorModelRef::removeMarkers()
{
  for(auto &marker : _markers) {
    marker->parent()->remove(marker);
  }
  _markers.clear();
}

void HingeEditorModelRef::updateAnimation()
{
  if (_physicsScene) {

    for (auto &hinge : _hinges) {
      const rp3d::Quaternion &q1 = hinge.elementBody->getTransform().getOrientation();
      const rp3d::Quaternion &q2 = hinge.anchorBody->getTransform().getOrientation();
      float hingeAngle = hinge.joint->computeCurrentHingeAngle(q1, q2);

      qDebug() << "angle:" << hingeAngle;
    }

    _physicsScene->update();
  }
}

void HingeEditorModelRef::checkPhysicsScene()
{
  if(!_physicsScene) {
    // Gravity vector in the dynamics world
    rp3d::Vector3 gravity(0, rp3d::decimal(-9.81), 0);

    rp3d::WorldSettings worldSettings;
    worldSettings.worldName = "HingeWorld";
    worldSettings.isSleepingEnabled = false;

    rp3d::DynamicsWorld *dynamicsWorld = new rp3d::DynamicsWorld(gravity, worldSettings);//, logger);
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

  for(const auto &hinge : _hinges) {
    QJsonObject hingeObject;

    switch(hinge.hingeType) {
      case HingeType::DOOR:
        hingeObject["name"] = "element";
        break;
      case HingeType::PROPELLER:
        hingeObject["name"] = "propeller";
        break;
    }
    hingeObject["name"] = QString::fromStdString(hinge.element->name());
    hingeObject["anchor"] = QString::fromStdString(hinge.anchor->name());
    hingeObject["element"] = QString::fromStdString(hinge.element->name());
    QJsonObject pointObject;
    pointObject["x"] = hinge.hingePoint.x;
    pointObject["y"] = hinge.hingePoint.y;
    pointObject["z"] = hinge.hingePoint.z;
    hingeObject["point"] = pointObject;
    QJsonObject axisObject;
    axisObject["x"] = hinge.hingeAxis.x;
    axisObject["y"] = hinge.hingeAxis.y;
    axisObject["z"] = hinge.hingeAxis.z;
    hingeObject["axis"] = axisObject;
    hingeObject["minAngleLimit"] = hinge.minAngleLimit;
    hingeObject["maxAngleLimit"] = hinge.maxAngleLimit;

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
  if(hingeRef.isNull()) {
    qCritical("Not a hinge definition file.");
    return false;
  }

  resetAll();
  checkPhysicsScene();

  const QJsonArray hingeArray = hingeRef.toArray();

  for(int i=0; i<hingeArray.size(); i++) {
    QJsonObject hingeObject = hingeArray[i].toObject();

    HingeType hingeType;
    const auto ht = hingeObject["name"].toString();
    if(ht == "element")
      hingeType = HingeType::DOOR;
    else if(ht == "propeller")
      hingeType = HingeType::PROPELLER;
    else
      throw std::invalid_argument("unknown hinge type");

    _hinges.emplace_back(HingeData(hingeType));
    HingeData &hingeData = _hinges.back();

    const QString hingeName = hingeObject["name"].toString();
    _hingeNames.append(hingeName);

    const QString anchorName = hingeObject["anchor"].toString();
    hingeData.anchor = _object->getChildByName(anchorName.toStdString());
    const QString elementName = hingeObject["element"].toString();
    hingeData.element = _object->getChildByName(elementName.toStdString());

    if(!hingeData.element || !hingeData.anchor) {

      qCritical() << "Hinge definition does not match model: element" << (!hingeData.element ? elementName : anchorName)
                  << "not found";
      _hinges.pop_back();
      return false;
    }
    const QJsonObject pointObject = hingeObject["point"].toObject();
    hingeData.hingePoint.setAllValues(pointObject["x"].toDouble(),
                                      pointObject["y"].toDouble(),
                                      pointObject["z"].toDouble());

    const QJsonObject axisObject = hingeObject["axis"].toObject();
    hingeData.hingeAxis.setAllValues(axisObject["x"].toDouble(),
                                     axisObject["y"].toDouble(),
                                     axisObject["z"].toDouble());

    hingeData.minAngleLimit = hingeObject["minAngleLimit"].toDouble();
    hingeData.maxAngleLimit = hingeObject["maxAngleLimit"].toDouble();

    createHingePhysics(hingeData);
  }
  emit hingeNamesChanged();

  return true;
}

bool HingeEditorModelRef::deleteHinge(const QString &name)
{
  std::string qname = name.toStdString();
  for(auto it = _hinges.begin(); it != _hinges.end(); it++) {
    const auto &hinge = *it;
    if(hinge.name == qname) {
      _physicsScene->remove(hinge.joint);
      _physicsScene->remove(hinge.element);

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

  if(!td || !tc) {
    qCritical() << "createHinge: null parameter";
    return;
  }

  //make sure the scene exists
  checkPhysicsScene();

  _hinges.emplace_back(HingeData(HingeType::DOOR));
  HingeData &hinge = _hinges.back();

  hinge.anchor = tc->object();
  hinge.element = td->object();

  //place the hinge in the middle between the hinge points
  const auto pt = (upper + lower) * 0.5;
  hinge.hingePoint.setAllValues(pt.x(), pt.y(), pt.z());

  //calculate the hinge axis
  const auto ax = (upper - lower).normalized();
  hinge.hingeAxis.setAllValues(ax.x(), ax.y(), ax.z());

  hinge.minAngleLimit = -(M_PI_2 * 0.8);
  hinge.maxAngleLimit = 0;

  createHingePhysics(hinge);

  _hingeNames.append(td->name());
  emit hingeNamesChanged();
}

void HingeEditorModelRef::createHingePhysics(HingeData &hinge)
{
  //the anchor is represented by the default shape (bounding box)
  tr3::PhysicsObject *anchorPhysics = _physicsScene->getPhysics(hinge.anchor, true, true);
  anchorPhysics->body()->setType(rp3d::BodyType::STATIC);
  hinge.anchorBody = anchorPhysics->body();

  //create the moving element and keep a reference
  tr3::PhysicsObject *elementPhysics = _physicsScene->getPhysics(hinge.element, true, true);
  hinge.elementBody = elementPhysics->body();

  rp3d::HingeJointInfo jointInfo(hinge.elementBody, anchorPhysics->body(),
                                 rp3d::Vector3(hinge.hingePoint.x, hinge.hingePoint.y, hinge.hingePoint.z),
                                 rp3d::Vector3(hinge.hingeAxis.x, hinge.hingeAxis.y, hinge.hingeAxis.z));

  jointInfo.isLimitEnabled = true;//hinge.maxAngleLimit != hinge.minAngleLimit;
  jointInfo.isMotorEnabled = true;
  jointInfo.motorSpeed = rp3d::decimal(0.1) * M_PI;
  jointInfo.maxMotorTorque = rp3d::decimal(30.0);
  jointInfo.minAngleLimit = hinge.minAngleLimit;
  jointInfo.maxAngleLimit = hinge.maxAngleLimit;
  jointInfo.isCollisionEnabled = false;

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
  marker->setName("TestMarker");

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