//
// Created by byter on 11/9/18.
//

#include "Dynamics.h"
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

Scene *Dynamics::getThreeScene()
{
  Scene *scene = dynamic_cast<Scene *>(parent());
  if(!scene) {
    qCritical() << "unable to find parent Scene";
    return nullptr;
  }
  return scene;
}

void Hinge::rotate(float angle) const
{
  float theta = direction == Direction::CLOCKWISE ? -angle : angle;

  auto wpos = element->parent()->localToWorld(element->position());
  wpos -= pointWorld;
  wpos.apply(axisWorld, theta);
  wpos += pointWorld;
  element->position() = element->parent()->worldToLocal(wpos);

  element->rotateOnAxis(axisLocal, theta);
  element->updateMatrix();
}


void Dynamics::update()
{
  auto elapsed = _timer.restart();
  if(elapsed > 0) {
    for(auto &hinge : _hinges) {
      float angle = float(M_PI * hinge.upm / 60000.0 * elapsed);
      if(hinge.angleLimit > 0 && hinge.rotatedAngle + angle > hinge.angleLimit) continue;

      hinge.rotatedAngle += angle;
      hinge.rotate(angle);
    }
  }
}

void Dynamics::fastforward(float seconds)
{
  for(auto &hinge : _hinges) {

    float angle = float(M_PI * hinge.upm / 60000.0 * seconds);
    if(hinge.angleLimit > 0 && hinge.rotatedAngle + angle > hinge.angleLimit) continue;

    hinge.rotatedAngle += angle;
    hinge.rotate(angle);
  }
}

QStringList Dynamics::hingeNames() const
{
  QStringList names;
  for(const auto &hinge : _hinges) names.append(QString::fromStdString(hinge.name));
  return names;
}

bool Dynamics::deleteHinge(const QString &name)
{
  std::string qname = name.toStdString();
  for (auto it = _hinges.begin(); it != _hinges.end(); it++) {
    const auto &hinge = *it;
    if (hinge.name == qname) {
      _hinges.erase(it);

      emit hingesChanged();
      return true;
    }
  }
  return false;
}

void Dynamics::loadHinges(const QJsonValueRef &json, Object3D::Ptr object)
{
  resetAll();

  const QJsonArray hingeArray = json.toArray();

  for (int i = 0; i < hingeArray.size(); i++) {
    QJsonObject hingeObject = hingeArray[i].toObject();

    Hinge::Type hingeType;
    const auto ht = hingeObject["type"].toString();
    if (ht == "door")
      hingeType = Hinge::Type::DOOR;
    else if (ht == "propeller")
      hingeType = Hinge::Type::PROPELLER;
    else
      throw std::invalid_argument("unknown hinge type");

    _hinges.emplace_back();
    Hinge &hinge = _hinges.back();

    hinge.type = hingeType;
    hinge.name = hingeObject["name"].toString().toStdString();

    const QString anchorName = hingeObject["anchor"].toString();
    const auto an = anchorName.toStdString();
    hinge.anchor = an == object->name() ? object : object->getChildByName(an);

    const QString elementName = hingeObject["element"].toString();
    const auto en = elementName.toStdString();
    hinge.element = en == object->name() ? object : object->getChildByName(en);

    if (!hinge.element || !hinge.anchor) {

      qCritical() << "Hinge definition does not match model: " << (!hinge.element ? elementName : anchorName)
                  << "not found";
      _hinges.pop_back();
      return;
    }

    const QJsonObject pointObject = hingeObject["point1"].toObject();
    hinge.point1.set(pointObject["x"].toDouble(),
                           pointObject["y"].toDouble(),
                           pointObject["z"].toDouble());

    const QJsonObject axisObject = hingeObject["point2"].toObject();
    hinge.point2.set(axisObject["x"].toDouble(),
                           axisObject["y"].toDouble(),
                           axisObject["z"].toDouble());

    hinge.angleLimit = hingeObject["angleLimit"].toDouble();

    setupHinge(hinge);
  }
  emit hingesChanged();
}

bool Dynamics::load(const QString &file, ThreeQObject *object)
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

Hinge &Dynamics::createHinge(Hinge::Type type, QVariant element, QVariant body, QVector3D one, QVector3D two)
{
  auto *elem = element.value<quick::ThreeQObject *>();
  auto *bdy = body.value<quick::ThreeQObject *>();

  if (!elem || !bdy) {
    throw std::invalid_argument("createHinge: null parameter");
  }

  _hinges.emplace_back();
  Hinge &hinge = _hinges.back();

  hinge.type = type;
  hinge.anchor = bdy->object();
  hinge.element = elem->object();

  //save as local points
  math::Vector3 ptw1(one.x(), one.y(), one.z());
  hinge.point1 = hinge.anchor->worldToLocal(ptw1);
  math::Vector3 ptw2(two.x(), two.y(), two.z());
  hinge.point2 = hinge.anchor->worldToLocal(ptw2);

  setupHinge(hinge);

  emit hingesChanged();
  return hinge;
}

void Dynamics::createPropellerHinge(QVariant propeller, QVariant body, QVector3D back, QVector3D front)
{
  createHinge(Hinge::Type::PROPELLER, propeller, body, back, front);
}

void Dynamics::createDoorHinge(QVariant door, QVariant body, QVector3D upper, QVector3D lower)
{
  Hinge &hinge = createHinge(Hinge::Type::DOOR, door, body, upper, lower);
  hinge.angleLimit = float(M_PI_2 * 0.8);
}

void Dynamics::setupHinge(Hinge &hinge)
{
  //calculate hinge axis and hinge point
  const auto hp1 = hinge.anchor->localToWorld(hinge.point1);
  const auto hp2 = hinge.anchor->localToWorld(hinge.point2);

  hinge.axisWorld = (hp1 - hp2).normalized();
  hinge.axisLocal = (hinge.element->worldToLocal(hp1) - hinge.element->worldToLocal(hp2)).normalized();
  hinge.pointWorld = (hp1 + hp2) * 0.5;

  //calculate position of door to determine turning direction
  const auto &hingePoint = (hinge.point1 + hinge.point2) * 0.5;

  const auto anchorCenter = hinge.anchor->computeBoundingBox().getCenter();
  const auto elementCenter = hinge.element->computeBoundingBox().getCenter();

  const auto anchorToHinge = (hingePoint - hinge.anchor->worldToLocal(anchorCenter)).normalized();
  const auto elementToHinge = (hingePoint - hinge.anchor->worldToLocal(elementCenter)).normalized();

  const auto cross = math::cross(anchorToHinge, elementToHinge);
  float leftRight = math::dot(cross, hinge.point1 - hinge.point2);

  hinge.direction = leftRight < 0.0f ? Hinge::Direction::CLOCKWISE : Hinge::Direction::COUNTERCLOCKWISE;
}

void Dynamics::resetAll()
{
  _hinges.clear();
  emit hingesChanged();
}


}
}
