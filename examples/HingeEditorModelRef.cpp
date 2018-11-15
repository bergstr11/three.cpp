//
// Created by byter on 10/23/18.
//

#include "HingeEditorModelRef.h"

#include <threepp/quick/interact/ObjectPicker.h>
#include <threepp/geometry/Cylinder.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/core/Geometry.h>
#include <threepp/quick/scene/R3dPhysics.hxx>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace three {
namespace quick {

using namespace three::react3d;

Object3D::Ptr HingeEditorModelRef::_create()
{
  _physics = dynamic_cast<r3d::Physics *>(_scene->physics());
  if(!_physics) {
    qCritical() << "no matching physics configuration found on scene";
  }
  return nullptr;
}

void HingeEditorModelRef::resetAll()
{
  _markers.clear();
  _physics->resetAll();
}

void HingeEditorModelRef::removeMarkers()
{
  for (auto &marker : _markers) {
    marker->parent()->remove(marker);
  }
  _markers.clear();
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

  for (const auto &hinge : _physics->hinges()) {
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
