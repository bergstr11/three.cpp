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
  enum class Position {FRONTLEFT, FRONTRIGHT, BACKLEFT, BACKRIGHT};

  Object3D::Ptr leftWheel;
  Object3D::Ptr rightWheel;
  Object3D *parent = nullptr;

  Vector3 parentCenter;
  Vector3 leftCenter;
  Vector3 rightCenter;
  Vector3 front;

  Vector3 leftAxis;
  Vector3 rightAxis;

  Position pos;
  int dir = 1;

  Vector3 leftCenterWorld;
  Vector3 rightCenterWorld;
  Vector3 leftAxisWorld;
  Vector3 rightAxisWorld;

  void rotate(float theta, Object3D::Ptr wheel, const Vector3 &axis, const Vector3 &axisWorld, const Vector3 &centerWorld) const
  {
    auto wpos = wheel->parent()->localToWorld(wheel->position());
    wpos -= centerWorld;
    wpos.apply(axisWorld, theta);
    wpos += centerWorld;
    wheel->position() = wheel->parent()->worldToLocal(wpos);

    wheel->rotateOnAxis(axis, theta);
    wheel->updateMatrix();
  }
  
  void rotate(float angle) const override
  {
    rotate(angle * dir, leftWheel, leftAxis, leftAxisWorld, leftCenterWorld);
    rotate(-angle * dir, rightWheel, rightAxis, rightAxisWorld, rightCenterWorld);
  }

  const char *toString(WheelHinge::Position pos)
  {
    switch(pos) {
      case WheelHinge::Position::BACKLEFT: return "BACKLEFT";
      case WheelHinge::Position::BACKRIGHT: return "BACKRIGHT";
      case WheelHinge::Position::FRONTLEFT: return "FRONTLEFT";
      case WheelHinge::Position::FRONTRIGHT: return "FRONTRIGHT";
      default: return "unknown";
    }
  }

  void update(const Quaternion &)
  {
    leftCenterWorld = leftWheel->localToWorld(leftCenter);
    rightCenterWorld = rightWheel->localToWorld(rightCenter);

    leftAxisWorld = (leftCenterWorld - rightCenterWorld).normalized();
    rightAxisWorld = (rightCenterWorld - leftCenterWorld).normalized();
  }

  WheelHinge(Object3D::Ptr leftWheel, Object3D::Ptr rightWheel, const Vector3 &front, Object3D *parent)
  : leftWheel(leftWheel), rightWheel(rightWheel), front(front.x(), front.y(), front.z()), parent(parent)
  {
    upm = 100;

    //save wheel midpoints as local coordinates
    Box3 leftBox = leftWheel->computeBoundingBox();
    leftCenter = leftWheel->worldToLocal(leftBox.getCenter());
    Box3 rightBox = rightWheel->computeBoundingBox();
    rightCenter  = rightWheel->worldToLocal(rightBox.getCenter());

    //save parent center as local coordinate
    const auto parentBox = parent->computeBoundingBox();
    parentCenter = parent->worldToLocal(parentBox.getCenter());

    update(parent->quaternion());

    {
      const auto leftCenter = parent->worldToLocal(leftCenterWorld);
      const auto rightCenter = parent->worldToLocal(rightCenterWorld);
      const auto leftToCenter = (parentCenter - leftCenter).normalized();
      const auto rightToCenter = (parentCenter - rightCenter).normalized();
      const auto midLocal = (leftCenter + rightCenter) * 0.5;

      Vector3 up(0, 0, 1);
      up.apply(parent->quaternion());
      auto crossVal = cross(leftToCenter, rightToCenter);
      float leftRight = dot(crossVal, up);

      //determine in what direction we're oriented
      float parentFront = (parentCenter - front).inclination();
      float localFront = (parentCenter - midLocal).inclination();
      bool isFront = parentFront < 0 ? localFront < 0 : localFront > 0;
      bool isRight = parentFront < 0 ? leftRight < 0 : leftRight > 0;
      pos = isRight ?
               (isFront ? WheelHinge::Position::FRONTRIGHT : WheelHinge::Position::BACKRIGHT) :
               (isFront ? WheelHinge::Position::FRONTLEFT : WheelHinge::Position::BACKLEFT);

      qDebug() << toString(pos) << leftRight << isFront << localFront << parentFront;

      dir = isFront ? 1 : -1;
    }

    switch(pos) {
      case WheelHinge::Position::BACKLEFT:
      case WheelHinge::Position::BACKRIGHT:
        dir = -1;
        break;
      case WheelHinge::Position::FRONTRIGHT:
      case WheelHinge::Position::FRONTLEFT:
        dir = 1;
        break;
    }
    if(leftWheel->position().isNull() && rightWheel->position().isNull()) {
      leftAxis = (leftCenter - leftWheel->worldToLocal(rightCenterWorld)).normalized();
      rightAxis = (rightCenter - rightWheel->worldToLocal(leftCenterWorld)).normalized();
    }
    else if(pos == Position::FRONTRIGHT || pos == Position::BACKLEFT) {
      dir = -dir;
      leftAxis = (leftWheel->worldToLocal(rightCenterWorld) - leftCenter).normalized();
      rightAxis = (rightCenter - rightWheel->worldToLocal(leftCenterWorld)).normalized();
    }
    else if(pos == Position::BACKRIGHT || pos == Position::FRONTLEFT) {
      if(pos == Position::FRONTLEFT) dir = -dir;
      leftAxis = (leftCenter - leftWheel->worldToLocal(rightCenterWorld)).normalized();
      rightAxis = (rightWheel->worldToLocal(leftCenterWorld) - rightCenter).normalized();
    }

    parent->quaternion().onChange.connect(this, &WheelHinge::update);
  }

  using Ptr = std::shared_ptr<WheelHinge>;
  static Ptr make(Object3D::Ptr leftWheel, Object3D::Ptr rightWheel, const Vector3 &front, Object3D *parent)
  {
    return Ptr(new WheelHinge(leftWheel, rightWheel, front, parent));
  }
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

    const auto ht = hingeObject["type"].toString();
    if (ht == "door")
      defaultHinge = DefaultHinge::makeDoor();
    else if (ht == "propeller")
      defaultHinge = DefaultHinge::makePropeller();
    else if (ht == "wheel") {
      const QString parentName = hingeObject["parent"].toString();
      const auto pn = parentName.toStdString();
      Object3D::Ptr parent = pn == object->name() ? object : object->getChildByName(pn);

      const QString leftName = hingeObject["left"].toString();
      const auto an = leftName.toStdString();
      Object3D::Ptr leftWheel = an == object->name() ? object : object->getChildByName(an);

      const QString rightName = hingeObject["right"].toString();
      const auto en = rightName.toStdString();
      Object3D::Ptr rightWheel = en == object->name() ? object : object->getChildByName(en);

      const QJsonObject frontObject = hingeObject["front"].toObject();
      Vector3 front(frontObject["x"].toDouble(), frontObject["y"].toDouble(), frontObject["z"].toDouble());

      if (!leftWheel || !rightWheel) {

        qCritical() << "Hinge definition does not match model: " << (!leftWheel ? leftName : rightName)
                    << "not found";
        _hinges.pop_back();
        return;
      }

      WheelHinge::Ptr wheelHinge = WheelHinge::make(leftWheel, rightWheel, front, parent.get());

      wheelHinge->name = hingeObject["name"].toString().toStdString();
      _hinges.push_back(wheelHinge);
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
      hingeObject["parent"] = QString::fromStdString(whinge->parent->name());
      hingeObject["left"] = QString::fromStdString(whinge->leftWheel->name());
      hingeObject["right"] = QString::fromStdString(whinge->rightWheel->name());

      QJsonObject frontObject;
      frontObject["x"] = whinge->front.x();
      frontObject["y"] = whinge->front.y();
      frontObject["z"] = whinge->front.z();
      hingeObject["leftCenter"] = frontObject;
    }
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

void Dynamics::createWheelHinge(QVariant left, QVariant right, QVector3D front)
{
  auto *leftWheel = left.value<quick::ThreeQObject *>();
  auto *rightWheel = right.value<quick::ThreeQObject *>();

  if (!leftWheel || !rightWheel) {
    qCritical() << "createWheelHinge: null parameter";
    return;
  }
  Object3D::Ptr leftW = leftWheel->object();
  Object3D::Ptr rightW = rightWheel->object();

  //find common parent for wheels
  Object3D *parent=nullptr, *wl=leftW.get(), *wr=rightW.get();
  for(int i=0; wl->parent() && wr->parent(); i++) {
    if(wl->parent() == wr->parent()) {
      parent = wl->parent();
      break;
    }
    if(i % 2 == 0)
      wl = wl->parent();
    else
      wr = wr->parent();
  }
  if(!parent) {
    qCritical() << "createWheelHinge: cannot determine common parent";
    return;
  }
  Vector3 pfront = parent->worldToLocal(Vector3(front.x(), front.y(), front.z()));

  WheelHinge::Ptr hinge = WheelHinge::make(leftW, rightW, pfront, parent);
  _hinges.push_back(hinge);

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
  const auto hingePoint = (hinge->point1 + hinge->point2) * 0.5;

  const auto anchorCenter = hinge->anchor->computeBoundingBox().getCenter();
  const auto elementCenter = hinge->element->computeBoundingBox().getCenter();

  const auto anchorToHinge = (hingePoint - hinge->anchor->worldToLocal(anchorCenter)).normalized();
  const auto elementToHinge = (hingePoint - hinge->anchor->worldToLocal(elementCenter)).normalized();

  const auto crossVal = cross(anchorToHinge, elementToHinge);
  float leftRight = dot(crossVal, hinge->point1 - hinge->point2);

  hinge->direction = leftRight < 0.0f ? Hinge::Direction::CLOCKWISE : Hinge::Direction::COUNTERCLOCKWISE;
}

void Dynamics::resetAll()
{
  _hinges.clear();
  emit hingesChanged();
}

}
}
