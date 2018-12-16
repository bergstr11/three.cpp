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

void HingeEditorModelRef::setDynamics(QObject *dynamics)
{
  if(_dynamics != dynamics) {
    _dynamics = dynamic_cast<Dynamics *>(dynamics);
    if(!_dynamics) {
      qCritical() << "cannot assign dynamics. Incompatible type";
      return;
    }
    emit dynamicsChanged();
  }
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
