//
// Created by cse on 5/12/17.
//

#ifndef THREEPP_QUICK_SCENE_H
#define THREEPP_QUICK_SCENE_H

#include <QQuickFramebufferObject>
#include <QMetaObject>
#include <QColor>
#include <memory>
#include <scene/Scene.h>
#include <camera/Camera.h>
#include "quick/ThreeQObjectRoot.h"
#include "quick/objects/ThreeQObject.h"
#include "quick/cameras/Camera.h"
#include "quick/elements/Fog.h"
#include "quick/interact/Controller.h"

namespace three {
namespace quick {

class Scene : public ThreeQObjectRoot
{
Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
  Q_PROPERTY(Camera * camera READ camera WRITE setCamera NOTIFY cameraChanged)
  Q_PROPERTY(FogBase * fog READ fog WRITE setFog NOTIFY fogChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeQObject> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QString _name;
  QColor _background;

  QList<ThreeQObject *> _objects;

  Camera *_quickCamera = nullptr;
  FogBase * _fog = nullptr;

  three::Camera::Ptr _camera;
  three::Scene::Ptr _scene;

  static void append_object(QQmlListProperty<ThreeQObject> *list, ThreeQObject *obj);
  static int count_objects(QQmlListProperty<ThreeQObject> *);
  static ThreeQObject *object_at(QQmlListProperty<ThreeQObject> *, int);
  static void clear_objects(QQmlListProperty<ThreeQObject> *);

  QQmlListProperty<ThreeQObject> objects();

protected:
  three::Object3D::Ptr _create(Scene *scene) {return nullptr;}

public:
  explicit Scene(QObject *parent=nullptr) : ThreeQObjectRoot(parent) {}

  ~Scene() override {};

  QColor background() { return _background; }

  void setBackground(const QColor &background);

  QString name() {return _name;}

  void setName(const QString &name);

  Camera *camera() {return _quickCamera;}

  void setCamera(Camera *camera);

  FogBase *fog() {return _fog;}

  void setFog(FogBase *fog);

  three::Scene::Ptr scene() {return _scene;}

  void addTo(ObjectRootContainer *container) override;

signals:
  void backgroundChanged();
  void nameChanged();
  void cameraChanged();
  void fogChanged();
};

}
}

#endif //THREEPP_QUICK_SCENE_H
