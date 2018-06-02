//
// Created by byter on 2/9/18.
//

#ifndef THREE_PPQ_THREEDITEM_H
#define THREE_PPQ_THREEDITEM_H

#include <QQuickFramebufferObject>
#include <QJSValue>
#include <threepp/renderers/OpenGLRenderer.h>
#include "Three.h"

namespace three {
namespace quick {

class ThreeDItem;
class ThreeQObject;
class ThreeQObjectRoot;
class Camera;
class Scene;

class Interactor
{
protected:
  ThreeDItem *_item = nullptr;
  bool _enabled = true;

  virtual bool handleMousePressed(QMouseEvent *event) {return false;}

  virtual bool handleMouseDoubleClicked(QMouseEvent *event) {return false;}

  virtual bool handleMouseMoved(QMouseEvent *event) {return false;}

  virtual bool handleMouseReleased(QMouseEvent *event) {return false;}

  virtual bool handleMouseWheel(QWheelEvent *event) {return false;}

public:
  void setItem(ThreeDItem *item);

  bool mousePressed(QMouseEvent *event) {
    return _enabled ? handleMousePressed(event) : false;
  }
  bool mouseDoubleClicked(QMouseEvent *event) {
    return _enabled ? handleMouseDoubleClicked(event) : false;
  }
  bool mouseMoved(QMouseEvent *event) {
    return _enabled ? handleMouseMoved(event) : false;
  }
  bool mouseReleased(QMouseEvent *event) {
    return _enabled ? handleMouseReleased(event) : false;
  }
  bool mouseWheel(QWheelEvent *event) {
    return _enabled ? handleMouseWheel(event) : false;
  }
};

class ThreeDItem : public QQuickFramebufferObject
{
Q_OBJECT
  friend class FramebufferObjectRenderer;
  friend class Scene;

private:
  Q_PROPERTY(three::quick::Three::ShadowType shadowType READ shadowType WRITE setShadowType NOTIFY shadowTypeChanged)
  Q_PROPERTY(three::quick::Three::CullFace faceCulling READ faceCulling WRITE setFaceCulling NOTIFY faceCullingChanged)
  Q_PROPERTY(three::quick::Three::FrontFaceDirection faceDirection READ faceDirection WRITE setFaceDirection NOTIFY faceDirectionChanged)
  Q_PROPERTY(bool autoClear READ autoClear WRITE setAutoClear NOTIFY autoClearChanged FINAL)
  Q_PROPERTY(bool autoRender READ autoRender WRITE setAutoRender NOTIFY autoRenderChanged FINAL)
  Q_PROPERTY(bool antialias READ antialias WRITE setAntialias NOTIFY antialiasChanged FINAL)
  Q_PROPERTY(unsigned samples READ samples WRITE setSamples NOTIFY samplesChanged FINAL)
  Q_PROPERTY(QRect viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
  Q_PROPERTY(QJSValue animate READ animate WRITE setAnimate NOTIFY animateChanged FINAL)
  Q_PROPERTY(unsigned fps READ fps WRITE setFps NOTIFY fpsChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeQObjectRoot> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QList<ThreeQObjectRoot *> _objects;

  std::vector<three::quick::Scene *> _scenes;

  std::vector<Interactor *> _interactors;

  struct RenderGroup {
    three::Scene::Ptr scene;
    three::Camera::Ptr camera;
    QJSValue prepare;

    RenderGroup(three::Scene::Ptr scene, three::Camera::Ptr camera, QJSValue prepare)
       : scene(scene), camera(camera), prepare(prepare) {}
  };
  std::vector<RenderGroup> _renderGroups;

  Three::ShadowType _shadowType = Three::None;
  Three::CullFace _faceCulling = Three::BackFaceCulling;
  Three::FrontFaceDirection _faceDirection = Three::FaceDirectionCCW;
  bool _autoClear = true, _autoRender = true, _antialias=false;
  unsigned _samples = 4;
  QRect _viewport;

  QTimer *_timer = nullptr;
  QJSValue _animateFunc;
  QJSValue _jsInstance;
  unsigned _fps = 60;

  three::OpenGLRenderer::Ptr _renderer;

  static void append_object(QQmlListProperty<ThreeQObjectRoot> *list, ThreeQObjectRoot *obj);
  static int count_objects(QQmlListProperty<ThreeQObjectRoot> *);
  static ThreeQObjectRoot *object_at(QQmlListProperty<ThreeQObjectRoot> *, int);
  static void clear_objects(QQmlListProperty<ThreeQObjectRoot> *);

  QQmlListProperty<ThreeQObjectRoot> objects();

public:
  explicit ThreeDItem(QQuickItem *parent = nullptr);

  ~ThreeDItem() override;

  Renderer *createRenderer() const override;

  void addScene(three::quick::Scene *scene);

  void addInteractor(Interactor *interactor);

  void removeInteractor(Interactor *controller);

  Three::ShadowType shadowType() const {return _shadowType;}

  void setShadowType(Three::ShadowType type);

  Three::CullFace faceCulling() const {return _faceCulling;}

  void setFaceCulling(Three::CullFace faceCulling);

  bool autoClear() const {return _autoClear;}

  void setAutoClear(bool autoClear);

  bool autoRender() const {return _autoRender;}

  void setAutoRender(bool autoRender);

  bool antialias() const {return _antialias;}

  void setAntialias(bool antialias);

  unsigned samples() const {return _samples;}

  void setSamples(unsigned samples);

  Three::FrontFaceDirection faceDirection() const {return _faceDirection;}

  void setFaceDirection(Three::FrontFaceDirection faceDirection);

  QRect viewport() {return _viewport;}

  void setViewport(QRect rect);

  QJSValue animate() {return _animateFunc;}

  void setAnimate(QJSValue animate);

  unsigned fps() const {return _fps;}

  void setFps(unsigned fps);

  Q_INVOKABLE void clear();

  Q_INVOKABLE void render(three::quick::Scene *scene, three::quick::Camera *camera, QJSValue prepare);

protected:
  bool execAnimate();

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

  void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

  void itemChange(ItemChange change, const ItemChangeData &data) override;

signals:
  void shadowTypeChanged();
  void faceCullingChanged();
  void faceDirectionChanged();
  void autoClearChanged();
  void autoRenderChanged();
  void samplesChanged();
  void antialiasChanged();
  void geometryChanged();
  void animateChanged();
  void viewportChanged();
  void fpsChanged();
};

}
}


#endif //THREE_PPQ_THREEDITEM_H
