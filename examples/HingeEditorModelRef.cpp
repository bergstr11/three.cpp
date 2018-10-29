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

    /*for (auto &hinge : _hinges) {
      hinge.doorBody->applyForceToCenterOfMass(rp3d::Vector3(0, _force, 0));
    }*/

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

    rp3d::DynamicsWorld *dynamicsWorld = new rp3d::DynamicsWorld(gravity, worldSettings);
    _physicsScene = react3d::PhysicsScene::make(_scene->scene(), dynamicsWorld);
  }
}

void HingeEditorModelRef::createHingePhysics(HingeData &hinge)
{
  //the door is represented by its bounding box
  const auto bb = hinge.door->computeBoundingBox().getSize();
  rp3d::BoxShape *doorBox = new rp3d::BoxShape(rp3d::Vector3(bb.x(), bb.y(), bb.z()));

  //a simple box will do for the car side
  rp3d::BoxShape *anchorBox = new rp3d::BoxShape(rp3d::Vector3(100, bb.y(), 100));
  rp3d::RigidBody *anchorBody = _physicsScene->getPhysics(hinge.car,
                                                          hinge.name,
                                                          hinge.hingePoint,
                                                          anchorBox,
                                                          rp3d::Transform::identity(), 1.0f);
  anchorBody->setType(rp3d::BodyType::STATIC);

  //create the door and keep a reference
  tr3::PhysicsObject *doorPhysics = _physicsScene->getPhysics(hinge.door, true);
  doorPhysics->body()->addCollisionShape(doorBox, rp3d::Transform::identity(), 1.0f);
  hinge.doorBody = doorPhysics->body();

  rp3d::HingeJointInfo jointInfo(anchorBody, hinge.doorBody,
                                 rp3d::Vector3(hinge.hingePoint.x, hinge.hingePoint.y, hinge.hingePoint.z),
                                 rp3d::Vector3(hinge.hingeAxis.x, hinge.hingeAxis.y, hinge.hingeAxis.z));

  jointInfo.isLimitEnabled = true;
  jointInfo.isMotorEnabled = true;
  jointInfo.motorSpeed = M_PI_2 / 6;
  jointInfo.minAngleLimit = hinge.minAngleLimit;
  jointInfo.maxAngleLimit = hinge.maxAngleLimit;
  jointInfo.isCollisionEnabled = false;

  // Create the joint in the dynamics world
  hinge.joint = _physicsScene->createHingeJoint(jointInfo);
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
    hingeObject["name"] = QString::fromStdString(hinge.door->name());
    hingeObject["car"] = QString::fromStdString(hinge.car->name());
    hingeObject["door"] = QString::fromStdString(hinge.door->name());
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

    _hinges.emplace_back(HingeData());
    HingeData &hingeData = _hinges.back();

    const QString hingeName = hingeObject["name"].toString();
    _hingeNames.append(hingeName);

    const QString carName = hingeObject["car"].toString();
    hingeData.car = _object->getChildByName(carName.toStdString());
    const QString doorName = hingeObject["door"].toString();
    hingeData.door = _object->getChildByName(doorName.toStdString());

    if(!hingeData.door || !hingeData.car) {
      qCritical("Hinge definition does not match model: door not found");
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
      _physicsScene->remove(hinge.door);

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

  _hinges.emplace_back(HingeData());
  HingeData &hinge = _hinges.back();

  hinge.car = tc->object();
  hinge.door = td->object();

  //place the hinge in the middle between the hinge points
  const auto pt = (upper + lower) * 0.5;
  hinge.hingePoint.setAllValues(pt.x(), pt.y(), pt.z());

  //calculate the hinge axis
  const auto ax = (lower - upper).normalized();
  hinge.hingeAxis.setAllValues(ax.x(), ax.y(), ax.z());

  hinge.minAngleLimit = -M_PI_2;
  hinge.maxAngleLimit = 0;

  createHingePhysics(hinge);

  _hingeNames.append(td->name());
  emit hingeNamesChanged();
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