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
#include "quick/objects/ThreeDObject.h"
#include "quick/cameras/Camera.h"
#include "quick/elements/Fog.h"
#include "quick/interact/Controller.h"

namespace three {
namespace quick {

class ThreeDScene : public QQuickFramebufferObject
{
  friend class FramebufferObjectRenderer;

Q_OBJECT
public:
  enum ShadowType {None, Basic, PCF, PCFSoft};
  Q_ENUM(ShadowType);

private:
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
  Q_PROPERTY(ShadowType shadowType READ shadowType WRITE setShadowType NOTIFY shadowTypeChanged)
  Q_PROPERTY(Camera * camera READ camera WRITE setCamera NOTIFY cameraChanged)
  Q_PROPERTY(FogBase * fog READ fog WRITE setFog NOTIFY fogChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeDObject> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QString _name;
  QColor _background;
  ShadowType _shadowType = None;

  QList<ThreeDObject *> _objects;

  Camera *_quickCamera = nullptr;
  FogBase * _fog = nullptr;

  three::Camera::Ptr _camera;
  three::Scene::Ptr _scene;

  std::vector<Controller *> _controllers;

  QMetaObject::Connection _geometryUpdate;

  static void append_object(QQmlListProperty<ThreeDObject> *list, ThreeDObject *obj);
  static int count_objects(QQmlListProperty<ThreeDObject> *);
  static ThreeDObject *object_at(QQmlListProperty<ThreeDObject> *, int);
  static void clear_objects(QQmlListProperty<ThreeDObject> *);

public:
  static void init();

  explicit ThreeDScene(QQuickItem *parent = 0);

  ~ThreeDScene() override;

  QQmlListProperty<ThreeDObject> objects();

  Renderer *createRenderer() const override;

  QColor background() { return _background; }

  void setBackground(const QColor &background);

  QString name() {return _name;}

  void setName(const QString &name);

  Camera *camera() {return _quickCamera;}

  void setCamera(Camera *camera);

  FogBase *fog() {return _fog;}

  void setFog(FogBase *fog);

  three::Scene::Ptr scene() {return _scene;}

  ShadowType shadowType() const {return _shadowType;}

  void setShadowType(ShadowType type);

  void addController(Controller *controller) {
    _controllers.push_back(controller);
  }

  void removeController(Controller *controller) {
    auto found = std::find(_controllers.begin(), _controllers.end(), controller);
    if(found != _controllers.end()) _controllers.erase(found);
  }

protected:
  void componentComplete() override;

  friend class FramebufferObjectRenderer;

  friend class RenderDelegate;

  void mouseMoveEvent(QMouseEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;

  void mouseReleaseEvent(QMouseEvent *event) override;

  void mouseDoubleClickEvent(QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void focusInEvent(QFocusEvent *event) override;

  void focusOutEvent(QFocusEvent *event) override;

  void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

  void releaseResources() override;

signals:
  void backgroundChanged();
  void nameChanged();
  void cameraChanged();
  void shadowTypeChanged();
  void fogChanged();

  void sceneGeometryChanged();
};

}
}

#endif //THREEPP_QUICK_SCENE_H
