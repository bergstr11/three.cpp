//
// Created by cse on 5/12/17.
//

#ifndef THREEPPQ_QUICK_SCENE_H
#define THREEPPQ_QUICK_SCENE_H

#include <QQmlListProperty>
#include <QMetaObject>
#include <QColor>
#include <threepp/scene/Scene.h>
#include <threepp/camera/Camera.h>
#include <threepp/quick/ThreeQObjectRoot.h>
#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/quick/cameras/Camera.h>
#include <threepp/quick/elements/Fog.h>
#include <threepp/quick/interact/Controller.h>

namespace three {
namespace quick {

class Scene : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
  Q_PROPERTY(Camera * camera READ quickCamera WRITE setQuickCamera NOTIFY cameraChanged)
  Q_PROPERTY(FogBase * fog READ fog WRITE setFog NOTIFY fogChanged)
  Q_PROPERTY(QVector3D position READ position NOTIFY positionChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeQObject> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QString _name;
  QColor _background;

  ThreeDItem * _item;

  QList<ThreeQObject *> _objects;

  Camera *_quickCamera = nullptr;
  FogBase * _fog = nullptr;

  QVector3D _position;

  three::Scene::Ptr _scene;

  static void append_object(QQmlListProperty<ThreeQObject> *list, ThreeQObject *obj);
  static int count_objects(QQmlListProperty<ThreeQObject> *);
  static ThreeQObject *object_at(QQmlListProperty<ThreeQObject> *, int);
  static void clear_objects(QQmlListProperty<ThreeQObject> *);

  QQmlListProperty<ThreeQObject> objects();

  void updateCamera();

public:
  explicit Scene(QObject *parent=nullptr);

  ~Scene() override {};

  QColor background() { return _background; }

  void setBackground(const QColor &background);

  QString name() {return _name;}

  void setName(const QString &name);

  Camera *quickCamera() {return _quickCamera;}

  void setQuickCamera(Camera *camera);

  FogBase *fog() {return _fog;}

  void setFog(FogBase *fog);

  QVector3D position() const {return _position;}

  three::Scene::Ptr scene() {return _scene;}

  three::Camera::Ptr camera() {return _quickCamera ? _quickCamera->camera() : nullptr;}

  Q_INVOKABLE void add(three::quick::ThreeQObject *object);

  Q_INVOKABLE void remove(three::quick::ThreeQObject *object);

  Q_INVOKABLE void clear();

  void setItem(ThreeDItem *item) override;

  ThreeDItem *item() {return _item;}

signals:
  void backgroundChanged();
  void nameChanged();
  void cameraChanged();
  void fogChanged();
  void positionChanged();
};

}
}

#endif //THREEPPQ_QUICK_SCENE_H
