//
// Created by byter on 10/23/18.
//

#include "HingeEditorModelRef.h"

#include <threepp/quick/interact/ObjectPicker.h>
#include <threepp/geometry/Cylinder.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/core/Geometry.h>
#include <threepp/quick/scene/Dynamics.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace three {
namespace quick {

Object3D::Ptr HingeEditorModelRef::_create()
{
  _dynamics = dynamic_cast<Dynamics *>(_scene->dynamics());
  if(!_dynamics) {
    qCritical() << "no matching dynamics configuration found on scene";
  }
  return nullptr;
}

void HingeEditorModelRef::resetAll()
{
  _markers.clear();
  _dynamics->resetAll();
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

  for (const auto &hinge : _dynamics->hinges()) {
    QJsonObject hingeObject;

    switch (hinge.type) {
      case Hinge::Type::DOOR:
        hingeObject["type"] = "door";
        break;
      case Hinge::Type::PROPELLER:
        hingeObject["type"] = "propeller";
        break;
    }

    hingeObject["name"] = QString::fromStdString(hinge.element->name());
    hingeObject["anchor"] = QString::fromStdString(hinge.anchor->name());
    hingeObject["element"] = QString::fromStdString(hinge.element->name());

    QJsonObject point1Object;
    point1Object["x"] = hinge.point1.x();
    point1Object["y"] = hinge.point1.y();
    point1Object["z"] = hinge.point1.z();
    hingeObject["point1"] = point1Object;

    QJsonObject point2Object;
    point2Object["x"] = hinge.point2.x();
    point2Object["y"] = hinge.point2.y();
    point2Object["z"] = hinge.point2.z();
    hingeObject["point2"] = point2Object;

    hingeObject["angleLimit"] = hinge.angleLimit;

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
