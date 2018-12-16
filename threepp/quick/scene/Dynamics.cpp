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

using namespace math;

struct DefaultHinge : public Hinge
{
  enum class Type {DOOR, PROPELLER};

  Type type;
  Object3D::Ptr anchor;
  Object3D::Ptr element;

  Direction direction;
  Vector3 axisWorld;
  Vector3 pointWorld;

  Vector3 axisLocal;
  Vector3 point1;
  Vector3 point2;

  void rotate(float angle) const override
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

  DefaultHinge(Type type) : type(type) {}

  using Ptr = std::shared_ptr<DefaultHinge>;
  static Ptr makeDoor() {return Ptr(new DefaultHinge(Type::DOOR));}
  static Ptr makePropeller() {return Ptr(new DefaultHinge(Type::PROPELLER));}
};

struct WheelHinge : public Hinge
{
  Object3D::Ptr leftWheel;
  Object3D::Ptr rightWheel;

  Vector3 leftMid;
  Vector3 rightMid;

  Vector3 leftMidWorld;
  Vector3 rightMidWorld;
  Vector3 axisWorld;

  Vector3 leftAxis;
  Vector3 rightAxis;

  void rotate(float theta, Object3D::Ptr wheel, const Vector3 &wheelAxis, const Vector3 &axisWorld, const Vector3 &midWorld) const
  {
    auto wpos = wheel->parent()->localToWorld(wheel->position());
    wpos -= midWorld;
    wpos.apply(axisWorld, theta);
    wpos += midWorld;
    wheel->position() = wheel->parent()->worldToLocal(wpos);

    wheel->rotateOnAxis(wheelAxis, theta);
    wheel->updateMatrix();
  }
  
  void rotate(float angle) const override
  {
    rotate(angle, leftWheel, leftAxis, axisWorld, leftMidWorld);
    rotate(angle, rightWheel, rightAxis, axisWorld, rightMidWorld);
  }

  using Ptr = std::shared_ptr<WheelHinge>;
  static Ptr make() {return Ptr(new WheelHinge());}
};

void setupHinge(DefaultHinge::Ptr hinge);
void setupHinge(WheelHinge::Ptr hinge);

void Dynamics::update()
{
  auto elapsed = _timer.restart();
  if(elapsed > 0) {
    for(auto &hinge : _hinges) {
      float angle = float(M_PI * hinge->upm / 60000.0 * elapsed);
      if(hinge->angleLimit > 0 && hinge->rotatedAngle + angle > hinge->angleLimit) continue;

      hinge->rotatedAngle += angle;
      hinge->rotate(angle);
    }
  }
}

void Dynamics::fastforward(float seconds)
{
  for(auto &hinge : _hinges) {

    float angle = float(M_PI * hinge->upm / 60.0 * seconds);
    if(hinge->angleLimit > 0) {
      float theta = std::min(abs(angle), hinge->angleLimit);
      angle = angle < 0 ? -theta : theta;
    }

    hinge->rotatedAngle += angle;
    hinge->rotate(angle);
  }
}

QStringList Dynamics::hingeNames() const
{
  QStringList names;
  for(const auto &hinge : _hinges) names.append(QString::fromStdString(hinge->name));
  return names;
}

bool Dynamics::deleteHinge(const QString &name)
{
  std::string qname = name.toStdString();
  for (auto it = _hinges.begin(); it != _hinges.end(); it++) {
    const auto &hinge = *it;
    if (hinge->name == qname) {
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

    DefaultHinge::Ptr defaultHinge;
    WheelHinge::Ptr wheelHinge;

    const auto ht = hingeObject["type"].toString();
    if (ht == "door")
      defaultHinge = DefaultHinge::makeDoor();
    else if (ht == "propeller")
      defaultHinge = DefaultHinge::makePropeller();
    else if (ht == "wheel") {
      wheelHinge = WheelHinge::make();
      continue;
    }
    else
      throw std::invalid_argument("unknown hinge type");

    if(defaultHinge) {
      _hinges.push_back(defaultHinge);

      defaultHinge->name = hingeObject["name"].toString().toStdString();

      const QString anchorName = hingeObject["anchor"].toString();
      const auto an = anchorName.toStdString();
      defaultHinge->anchor = an == object->name() ? object : object->getChildByName(an);

      const QString elementName = hingeObject["element"].toString();
      const auto en = elementName.toStdString();
      defaultHinge->element = en == object->name() ? object : object->getChildByName(en);

      if (!defaultHinge->element || !defaultHinge->anchor) {

        qCritical() << "Hinge definition does not match model: " << (!defaultHinge->element ? elementName : anchorName)
                    << "not found";
        _hinges.pop_back();
        return;
      }

      const QJsonObject pointObject = hingeObject["point1"].toObject();
      defaultHinge->point1.set(pointObject["x"].toDouble(),
                        pointObject["y"].toDouble(),
                        pointObject["z"].toDouble());

      const QJsonObject axisObject = hingeObject["point2"].toObject();
      defaultHinge->point2.set(axisObject["x"].toDouble(),
                        axisObject["y"].toDouble(),
                        axisObject["z"].toDouble());
      defaultHinge->angleLimit = hingeObject["angleLimit"].toDouble();

      setupHinge(defaultHinge);
    }
    else {
      _hinges.push_back(wheelHinge);

      wheelHinge->name = hingeObject["name"].toString().toStdString();

      const QString leftName = hingeObject["left"].toString();
      const auto an = leftName.toStdString();
      wheelHinge->leftWheel = an == object->name() ? object : object->getChildByName(an);

      const QString rightName = hingeObject["right"].toString();
      const auto en = rightName.toStdString();
      wheelHinge->rightWheel = en == object->name() ? object : object->getChildByName(en);

      if (!wheelHinge->leftWheel || !wheelHinge->rightWheel) {

        qCritical() << "Hinge definition does not match model: " << (!wheelHinge->leftWheel ? leftName : rightName)
                    << "not found";
        _hinges.pop_back();
        return;
      }

      const QJsonObject pointObject = hingeObject["leftMid"].toObject();
      wheelHinge->leftMid.set(pointObject["x"].toDouble(),
                              pointObject["y"].toDouble(),
                              pointObject["z"].toDouble());

      const QJsonObject axisObject = hingeObject["rightMid"].toObject();
      wheelHinge->rightMid.set(axisObject["x"].toDouble(),
                               axisObject["y"].toDouble(),
                               axisObject["z"].toDouble());

      wheelHinge->angleLimit = hingeObject["angleLimit"].toDouble();

      setupHinge(wheelHinge);
    }
  }
  emit hingesChanged();
}

bool Dynamics::load(const QString &file, ThreeQObject *object)
{
  QUrl fileUrl(file);
  QFile loadFile(fileUrl.isLocalFile() ? fileUrl.toLocalFile() : file);

  if (!loadFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Couldn't open file " << file;
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

bool Dynamics::save(const QString &file)
{
  QUrl fileUrl(file);
  QFile saveFile(fileUrl.isValid() ? fileUrl.toLocalFile() : file);

  if (!saveFile.open(QIODevice::WriteOnly)) {
    qCritical("Couldn't open file.");
    return false;
  }

  QJsonArray hingeArray;

  for (const auto &hinge : hinges()) {
    QJsonObject hingeObject;

    DefaultHinge::Ptr dhinge = std::dynamic_pointer_cast<DefaultHinge>(hinge);
    if(dhinge) {
      switch (dhinge->type) {
        case DefaultHinge::Type::DOOR:
          hingeObject["type"] = "door";
          break;
        case DefaultHinge::Type::PROPELLER:
          hingeObject["type"] = "propeller";
          break;
      }

      hingeObject["name"] = QString::fromStdString(dhinge->element->name());
      hingeObject["anchor"] = QString::fromStdString(dhinge->anchor->name());
      hingeObject["element"] = QString::fromStdString(dhinge->element->name());

      QJsonObject point1Object;
      point1Object["x"] = dhinge->point1.x();
      point1Object["y"] = dhinge->point1.y();
      point1Object["z"] = dhinge->point1.z();
      hingeObject["point1"] = point1Object;

      QJsonObject point2Object;
      point2Object["x"] = dhinge->point2.x();
      point2Object["y"] = dhinge->point2.y();
      point2Object["z"] = dhinge->point2.z();
      hingeObject["point2"] = point2Object;
    }
    else {
      WheelHinge::Ptr whinge = std::dynamic_pointer_cast<WheelHinge>(hinge);
      hingeObject["type"] = "wheel";

      hingeObject["name"] = QString::fromStdString(whinge->name);
      hingeObject["left"] = QString::fromStdString(whinge->leftWheel->name());
      hingeObject["right"] = QString::fromStdString(whinge->rightWheel->name());

      QJsonObject leftMidObject;
      leftMidObject["x"] = whinge->leftMid.x();
      leftMidObject["y"] = whinge->leftMid.y();
      leftMidObject["z"] = whinge->leftMid.z();
      hingeObject["leftMid"] = leftMidObject;

      QJsonObject rightMidObject;
      rightMidObject["x"] = whinge->rightMid.x();
      rightMidObject["y"] = whinge->rightMid.y();
      rightMidObject["z"] = whinge->rightMid.z();
      hingeObject["rightMid"] = rightMidObject;
    }
    hingeObject["angleLimit"] = hinge->angleLimit;

    hingeArray.push_back(hingeObject);
  }

  QJsonObject docObject;
  docObject["hinges"] = hingeArray;

  QJsonDocument saveDoc(docObject);
  saveFile.write(saveDoc.toJson());

  return true;
}

DefaultHinge::Ptr createDefaultHinge(DefaultHinge::Ptr hinge, QVariant element, QVariant body, QVector3D one, QVector3D two)
{
  auto *elem = element.value<quick::ThreeQObject *>();
  auto *bdy = body.value<quick::ThreeQObject *>();

  if (!elem || !bdy) {
    throw std::invalid_argument("createHinge: null parameter");
  }

  hinge->anchor = bdy->object();
  hinge->element = elem->object();

  //save as local points
  Vector3 ptw1(one.x(), one.y(), one.z());
  hinge->point1 = hinge->anchor->worldToLocal(ptw1);
  Vector3 ptw2(two.x(), two.y(), two.z());
  hinge->point2 = hinge->anchor->worldToLocal(ptw2);

  setupHinge(hinge);

  return hinge;
}

void Dynamics::createPropellerHinge(QVariant propeller, QVariant body, QVector3D back, QVector3D front)
{
  const auto hinge = createDefaultHinge(DefaultHinge::makePropeller(), propeller, body, back, front);
  _hinges.push_back(hinge);
  hinge->upm = 120;

  emit hingesChanged();
}

void Dynamics::createDoorHinge(QVariant door, QVariant body, QVector3D upper, QVector3D lower)
{
  DefaultHinge::Ptr hinge = createDefaultHinge(DefaultHinge::makeDoor(), door, body, upper, lower);
  _hinges.push_back(hinge);
  hinge->angleLimit = float(M_PI_2 * 0.8);

  emit hingesChanged();
}

void Dynamics::createWheelHinge(QVariant left, QVariant right, const QVector3D &leftMid, const QVector3D &rightMid)
{
  auto *leftWheel = left.value<quick::ThreeQObject *>();
  auto *rightWheel = right.value<quick::ThreeQObject *>();

  if (!leftWheel || !rightWheel) {
    throw std::invalid_argument("createWheelHinge: null parameter");
  }

  WheelHinge::Ptr hinge = WheelHinge::make();
  _hinges.push_back(hinge);

  hinge->upm = 100;
  hinge->leftWheel = leftWheel->object();
  hinge->rightWheel = rightWheel->object();

  //save as local points
  Vector3 ptw1(leftMid.x(), leftMid.y(), leftMid.z());
  hinge->leftMid = hinge->leftWheel->worldToLocal(ptw1);
  Vector3 ptw2(rightMid.x(), rightMid.y(), rightMid.z());
  hinge->rightMid  = hinge->rightWheel->worldToLocal(ptw2);

  setupHinge(hinge);

  emit hingesChanged();
}

void setupHinge(DefaultHinge::Ptr hinge)
{
  //calculate hinge axis and hinge point
  const auto hp1 = hinge->anchor->localToWorld(hinge->point1);
  const auto hp2 = hinge->anchor->localToWorld(hinge->point2);

  hinge->axisWorld = (hp1 - hp2).normalized();
  hinge->axisLocal = (hinge->element->worldToLocal(hp1) - hinge->element->worldToLocal(hp2)).normalized();
  hinge->pointWorld = (hp1 + hp2) * 0.5;

  //calculate position of door to determine turning direction
  const auto &hingePoint = (hinge->point1 + hinge->point2) * 0.5;

  const auto anchorCenter = hinge->anchor->computeBoundingBox().getCenter();
  const auto elementCenter = hinge->element->computeBoundingBox().getCenter();

  const auto anchorToHinge = (hingePoint - hinge->anchor->worldToLocal(anchorCenter)).normalized();
  const auto elementToHinge = (hingePoint - hinge->anchor->worldToLocal(elementCenter)).normalized();

  const auto crossVal = cross(anchorToHinge, elementToHinge);
  float leftRight = dot(crossVal, hinge->point1 - hinge->point2);

  hinge->direction = leftRight < 0.0f ? Hinge::Direction::CLOCKWISE : Hinge::Direction::COUNTERCLOCKWISE;
}

void setupHinge(WheelHinge::Ptr hinge)
{
  //calculate hinge axis and hinge point
  hinge->leftMidWorld = hinge->leftWheel->localToWorld(hinge->leftMid);
  hinge->rightMidWorld = hinge->rightWheel->localToWorld(hinge->rightMid);

  const auto &hl = hinge->leftMidWorld;
  const auto &hr = hinge->rightMidWorld;
  
  hinge->axisWorld = (hl - hr).normalized();
  hinge->leftAxis = (hinge->leftWheel->worldToLocal(hr) - hinge->leftMid).normalized();
  hinge->rightAxis = (hinge->rightWheel->worldToLocal(hl) - hinge->rightMid).normalized();
}

void Dynamics::resetAll()
{
  _hinges.clear();
  emit hingesChanged();
}


}
}
