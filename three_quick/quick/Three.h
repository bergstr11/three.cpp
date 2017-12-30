//
// Created by byter on 12/29/17.
//

#ifndef THREEPP_THREE_H
#define THREEPP_THREE_H

#include <QQuickFramebufferObject>
#include <QMetaObject>
#include <scene/Scene.h>
#include <camera/Camera.h>
#include "interact/Controller.h"

namespace three {
namespace quick {

class ThreeQObject;
class ThreeQObjectRoot;
class Scene;
class Material;
class Texture;
class Controller;

using scene_and_camera = std::pair<three::Scene::Ptr, three::Camera::Ptr>;

void init();

class Three : public QObject
{
Q_OBJECT
public:
  enum ShadowType {None, Basic, PCF, PCFSoft};
  Q_ENUM(ShadowType);

  enum Side {FrontSide, BackSide, FrontAndBackSide};
  Q_ENUM(Side);
};

class ObjectRootContainer {
public:
  virtual void addMaterial(three::Material::Ptr material) = 0;
  virtual void addTexture(three::Texture::Ptr texture) = 0;
  virtual void addController(Controller* controller, three::Camera::Ptr camera) = 0;
  virtual void addScene(three::Scene::Ptr scene, three::Camera::Ptr camera) = 0;
};

class ThreeDItem : public QQuickFramebufferObject, public ObjectRootContainer
{
Q_OBJECT
  friend class FramebufferObjectRenderer;
  friend class Scene;

private:
  Q_PROPERTY(three::quick::Three::ShadowType shadowType READ shadowType WRITE setShadowType NOTIFY shadowTypeChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeQObjectRoot> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QList<ThreeQObjectRoot *> _objects;

  std::vector<scene_and_camera> _scenes;

  std::vector<Controller *> _controllers;

  Three::ShadowType _shadowType = Three::None;

  QMetaObject::Connection _geometryUpdate;

  static void append_object(QQmlListProperty<ThreeQObjectRoot> *list, ThreeQObjectRoot *obj);
  static int count_objects(QQmlListProperty<ThreeQObjectRoot> *);
  static ThreeQObjectRoot *object_at(QQmlListProperty<ThreeQObjectRoot> *, int);
  static void clear_objects(QQmlListProperty<ThreeQObjectRoot> *);

  QQmlListProperty<ThreeQObjectRoot> objects();

public:
  explicit ThreeDItem(QQuickItem *parent = 0);

  ~ThreeDItem() override;

  Renderer *createRenderer() const override;

  void addController(Controller *controller, three::Camera::Ptr camera) override {
    controller->start(this, camera);
    _controllers.push_back(controller);
  }

  void removeController(Controller *controller) {
    auto found = std::find(_controllers.begin(), _controllers.end(), controller);
    if(found != _controllers.end()) _controllers.erase(found);
  }

  Three::ShadowType shadowType() const {return _shadowType;}

  void setShadowType(Three::ShadowType type);

  void addMaterial(three::Material::Ptr material) override;

  void addTexture(three::Texture::Ptr texture) override;

  void addScene(three::Scene::Ptr scene, three::Camera::Ptr camera) override;

protected:
  void componentComplete() override;

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
  void sceneGeometryChanged();
  void shadowTypeChanged();
};

}
}


#endif //THREEPP_THREE_H
