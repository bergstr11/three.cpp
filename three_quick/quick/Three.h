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

void init();

class Three : public QObject
{
Q_OBJECT
public:
  enum ShadowType {
    None = (unsigned)three::ShadowMapType::NoShadow,
    Basic = (unsigned)three::ShadowMapType::Basic,
    PCF  = (unsigned)three::ShadowMapType::PCF,
    PCFSoft  = (unsigned)three::ShadowMapType::PCFSoft
  };
  Q_ENUM(ShadowType);

  enum Side {
    FrontSide = (unsigned)three::Side::Front,
    BackSide = (unsigned)three::Side::Back,
    FrontAndBackSide = (unsigned)three::Side::Double
  };
  Q_ENUM(Side);

  enum CullFace
  {
    NoFaceCulling  = (unsigned)three::CullFace::None,
    BackFaceCulling = (unsigned)three::CullFace::Back,
    FrontFaceCulling = (unsigned)three::CullFace::Front,
    FrontBackFaceCulling = (unsigned)three::CullFace::FrontBack
  };
  Q_ENUM(CullFace);

  enum FrontFaceDirection
  {
    FaceDirectionCW = (unsigned)three::FrontFaceDirection::CW,
    FaceDirectionCCW = (unsigned)three::FrontFaceDirection::CCW,
    FaceDirectionUndefined = (unsigned)three::FrontFaceDirection::Undefined
  };
  Q_ENUM(FrontFaceDirection)
};

class ObjectRootContainer {
public:
  virtual void addMaterial(Material *material) = 0;
  virtual void addTexture(Texture *texture) = 0;
  virtual void addController(Controller* controller, three::Camera::Ptr camera) = 0;
  virtual void addScene(Scene *scene) = 0;
};

class ThreeDItem : public QQuickFramebufferObject, public ObjectRootContainer
{
Q_OBJECT
  friend class FramebufferObjectRenderer;
  friend class Scene;

private:
  Q_PROPERTY(three::quick::Three::ShadowType shadowType READ shadowType WRITE setShadowType NOTIFY shadowTypeChanged)
  Q_PROPERTY(three::quick::Three::CullFace faceCulling READ faceCulling WRITE setFaceCulling NOTIFY faceCullingChanged)
  Q_PROPERTY(three::quick::Three::FrontFaceDirection faceDirection READ faceDirection WRITE setFaceDirection NOTIFY faceDirectionChanged)
  Q_PROPERTY(bool autoClear READ autoClear WRITE setAutoClear NOTIFY autoClearChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeQObjectRoot> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QList<ThreeQObjectRoot *> _objects;

  std::vector<Scene *> _scenes;

  std::vector<Controller *> _controllers;

  Three::ShadowType _shadowType = Three::None;
  Three::CullFace _faceCulling = Three::NoFaceCulling;
  Three::FrontFaceDirection _faceDirection = Three::FaceDirectionCW;
  bool _autoClear = true;

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

  Three::CullFace faceCulling() const {return _faceCulling;}

  void setFaceCulling(Three::CullFace faceCulling);

  bool autoClear() const {return _autoClear;}

  void setAutoClear(bool autoClear);

  Three::FrontFaceDirection faceDirection() const {return _faceDirection;}

  void setFaceDirection(Three::FrontFaceDirection faceDirection);

  void addMaterial(Material *material) override;

  void addTexture(Texture *texture) override;

  void addScene(Scene *scene) override;

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
  void faceCullingChanged();
  void faceDirectionChanged();
  void autoClearChanged();
};

}
}


#endif //THREEPP_THREE_H
