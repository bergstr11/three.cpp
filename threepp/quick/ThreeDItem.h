//
// Created by byter on 2/9/18.
//

#ifndef THREE_PP_THREEDITEM_H
#define THREE_PP_THREEDITEM_H

#include <QQuickFramebufferObject>
#include <threepp/camera/Camera.h>
#include <threepp/renderers/OpenGLRenderer.h>
#include "Three.h"

namespace three {
namespace quick {

class ThreeQObject;
class ThreeQObjectRoot;

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
  Q_PROPERTY(unsigned samples READ samples WRITE setSamples NOTIFY samplesChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeQObjectRoot> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QList<ThreeQObjectRoot *> _objects;

  std::vector<Scene *> _scenes;

  std::vector<Controller *> _controllers;

  Three::ShadowType _shadowType = Three::None;
  Three::CullFace _faceCulling = Three::BackFaceCulling;
  Three::FrontFaceDirection _faceDirection = Three::FaceDirectionCCW;
  bool _autoClear = true;
  unsigned _samples = 4;

  three::OpenGLRenderer::Ptr _renderer;

  static void append_object(QQmlListProperty<ThreeQObjectRoot> *list, ThreeQObjectRoot *obj);
  static int count_objects(QQmlListProperty<ThreeQObjectRoot> *);
  static ThreeQObjectRoot *object_at(QQmlListProperty<ThreeQObjectRoot> *, int);
  static void clear_objects(QQmlListProperty<ThreeQObjectRoot> *);

  QQmlListProperty<ThreeQObjectRoot> objects();

public:
  explicit ThreeDItem(QQuickItem *parent = 0);

  ~ThreeDItem() override;

  Renderer *createRenderer() const override;

  void addScene(Scene *scene) override;

  void addController(Controller *controller) override;

  void removeController(Controller *controller);

  Three::ShadowType shadowType() const {return _shadowType;}

  void setShadowType(Three::ShadowType type);

  Three::CullFace faceCulling() const {return _faceCulling;}

  void setFaceCulling(Three::CullFace faceCulling);

  bool autoClear() const {return _autoClear;}

  void setAutoClear(bool autoClear);

  unsigned samples() const {return _samples;}

  void setSamples(unsigned samples);

  Three::FrontFaceDirection faceDirection() const {return _faceDirection;}

  void setFaceDirection(Three::FrontFaceDirection faceDirection);

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

  void releaseResources() override;

signals:
  void shadowTypeChanged();
  void faceCullingChanged();
  void faceDirectionChanged();
  void autoClearChanged();
  void samplesChanged();
};

}
}


#endif //THREE_PP_THREEDITEM_H
