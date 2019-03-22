//
// Created by byter on 2/9/18.
//

#ifndef THREE_PPQ_THREEDITEM_H
#define THREE_PPQ_THREEDITEM_H

#include <QQuickFramebufferObject>
#include <QJSValue>
#include <QTimer>
#include <threepp/renderers/OpenGLRenderer.h>
#include <threepp/util/simplesignal.h>
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
  bool _unifyClicked = false;
  QTimer _clickedTimer;
  QMouseEvent _clicked {QEvent::None, {0, 0}, Qt::NoButton, Qt::NoButton, Qt::NoModifier};

  virtual bool handleMousePressed(QMouseEvent *event) {return false;}

  virtual bool handleMouseReleased(QMouseEvent *event) {return false;}

  virtual bool handleMouseClicked(QMouseEvent *event) {return false;}

  virtual bool handleMouseDoubleClicked(QMouseEvent *event) {return false;}

  virtual bool handleMouseMoved(QMouseEvent *event) {return false;}

  virtual bool handleMouseWheel(QWheelEvent *event) {return false;}

  virtual bool handleTouchEvent(QTouchEvent *event) {return false;}

public:
  void setItem(ThreeDItem *item);

  /**
   * @param event
   * @return true if the item should be updated
   */
  bool mousePressed(QMouseEvent *event);

  /**
   * @param event
   * @return true if the item should be updated
   */
  bool mouseReleased(QMouseEvent *event);

  /**
   * @param event
   * @return true if the item should be updated
   */
  bool mouseDoubleClicked(QMouseEvent *event);

  /**
   * @param event
   * @return true if the item should be updated
   */
  bool mouseMoved(QMouseEvent *event);

  /**
   * @param event
   * @return true if the item should be updated
   */
  bool mouseWheel(QWheelEvent *event);

  /**
   * @param event
   * @return true if the item should be updated
   */
  bool touchEvent(QTouchEvent *event);
};

class ShadowMap : public QObject
{
  Q_OBJECT
  Q_PROPERTY(three::quick::Three::ShadowType type READ type WRITE setType NOTIFY typeChanged)
  Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
  Q_PROPERTY(bool needsUpdate READ needsUpdate WRITE setNeedsUpdate NOTIFY needsUpdateChanged)

  Three::ShadowType _shadowType = Three::None;
  bool _autoUpdate = true, _needsUpdate = false;
  three::OpenGLRenderer::Ptr _renderer;

public:
  ShadowMap(QObject *parent=nullptr) : QObject(parent) {}

  void setRenderer(three::OpenGLRenderer::Ptr renderer) {
    _renderer = renderer;
  }

  Three::ShadowType type() const {return _shadowType;}

  void setType(Three::ShadowType type);

  bool autoUpdate() const {return _autoUpdate;}

  void setAutoUpdate(bool autoUpdate);

  bool needsUpdate() const {return _needsUpdate;}

  void setNeedsUpdate(bool needsUpdate);

signals:
  void typeChanged();
  void autoUpdateChanged();
  void needsUpdateChanged();
};

class ThreeDItem : public QQuickFramebufferObject
{
Q_OBJECT
  friend class FramebufferObjectRenderer;
  friend class Scene;

private:
  Q_PROPERTY(three::quick::Three::CullFace faceCulling READ faceCulling WRITE setFaceCulling NOTIFY faceCullingChanged)
  Q_PROPERTY(three::quick::Three::FrontFaceDirection faceDirection READ faceDirection WRITE setFaceDirection NOTIFY faceDirectionChanged)
  Q_PROPERTY(bool autoClear READ autoClear WRITE setAutoClear NOTIFY autoClearChanged FINAL)
  Q_PROPERTY(bool autoRender READ autoRender WRITE setAutoRender NOTIFY autoRenderChanged FINAL)
  Q_PROPERTY(bool antialias READ antialias WRITE setAntialias NOTIFY antialiasChanged FINAL)
  Q_PROPERTY(unsigned samples READ samples WRITE setSamples NOTIFY samplesChanged FINAL)
  Q_PROPERTY(QRect viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
  Q_PROPERTY(QJSValue animate READ animate WRITE setAnimate NOTIFY animateChanged FINAL)
  Q_PROPERTY(bool autoAnimate READ autoAnimate WRITE setAutoAnimate NOTIFY autoAnimateChanged)
  Q_PROPERTY(three::quick::Three::ToneMapping toneMapping READ toneMapping WRITE setToneMapping NOTIFY toneMappingChanged)
  Q_PROPERTY(float toneMappingExposure READ toneMappingExposure WRITE setToneMappingExposure NOTIFY toneMappingExposureChanged)
  Q_PROPERTY(bool physicallyCorrectLights READ physicallyCorrectLights WRITE setPhysicallyCorrectLights NOTIFY physicallyCorrectLightsChanged)
  Q_PROPERTY(three::quick::ShadowMap *shadowMap READ shadowMap CONSTANT)
  Q_PROPERTY(unsigned fps READ fps WRITE setFps NOTIFY fpsChanged)
  Q_PROPERTY(ThreeDItem *usePrograms READ usePrograms WRITE setUsePrograms NOTIFY useProgramsChanged)
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

  Three::CullFace _faceCulling = Three::BackFaceCulling;
  Three::FrontFaceDirection _faceDirection = Three::FaceDirectionCCW;
  bool _autoClear = true, _autoRender = true, _antialias=false;
  unsigned _samples = 4;
  QRect _viewport;

  bool _autoAnimate = true;
  bool _pysicallyCorrectLights = false;
  three::quick::Three::ToneMapping _toneMapping = three::quick::Three::LinearToneMapping;
  float _toneMappingExposure = 1.0f;
  QTimer *_animateTimer = nullptr;
  QJSValue _animateFunc;
  QJSValue _jsInstance;
  unsigned _fps = 60;

  three::OpenGLRenderer::Ptr _renderer;

  ThreeDItem *_usePrograms = nullptr;

  ShadowMap _shadowMap;

  static void append_object(QQmlListProperty<ThreeQObjectRoot> *list, ThreeQObjectRoot *obj);
  static int count_objects(QQmlListProperty<ThreeQObjectRoot> *);
  static ThreeQObjectRoot *object_at(QQmlListProperty<ThreeQObjectRoot> *, int);
  static void clear_objects(QQmlListProperty<ThreeQObjectRoot> *);

  QQmlListProperty<ThreeQObjectRoot> objects();

public:
  Signal<void(OpenGLRenderer::Ptr, three::Renderer::Target::Ptr)> onRendered;

  explicit ThreeDItem(QQuickItem *parent = nullptr);

  ~ThreeDItem() override;

  Renderer *createRenderer() const override;

  void addScene(three::quick::Scene *scene);

  void addInteractor(Interactor *interactor);

  void removeInteractor(Interactor *controller);

  Three::CullFace faceCulling() const {return _faceCulling;}

  void setFaceCulling(Three::CullFace faceCulling);

  bool autoClear() const {return _autoClear;}

  void setAutoClear(bool autoClear);

  bool autoRender() const {return _autoRender;}

  void setAutoRender(bool autoRender);

  bool antialias() const {return _antialias;}

  void setAntialias(bool antialias);

  bool autoAnimate() const {return _autoAnimate;}

  void setAutoAnimate(bool autoAnimate);

  bool physicallyCorrectLights() const {return _pysicallyCorrectLights;}

  void setPhysicallyCorrectLights(bool pysicallyCorrectLights);

  three::quick::Three::ToneMapping toneMapping() {return _toneMapping;}

  void setToneMapping(three::quick::Three::ToneMapping toneMapping);

  float toneMappingExposure() {return _toneMappingExposure;}

  void setToneMappingExposure(float toneMappingExposure);

  unsigned samples() const {return _samples;}

  void setSamples(unsigned samples);

  Three::FrontFaceDirection faceDirection() const {return _faceDirection;}

  void setFaceDirection(Three::FrontFaceDirection faceDirection);

  QRect viewport() {return _viewport;}

  void setViewport(QRect rect);

  QJSValue animate() {return _animateFunc;}

  void setAnimate(QJSValue animate);

  unsigned fps() const {return _fps;}

  ThreeDItem *usePrograms() const {return _usePrograms;}

  void setUsePrograms(ThreeDItem *item);

  void setFps(unsigned fps);

  const ShadowMap *shadowMap() const {return &_shadowMap;}

  ShadowMap *shadowMap() {return &_shadowMap;}

  void lockWhile(std::function<void()>);

  Q_INVOKABLE void clear();

  Q_INVOKABLE void render(three::quick::Scene *scene, three::quick::Camera *camera, QJSValue prepare);

  Q_INVOKABLE void runAnimation(bool animate);

protected:
  bool execAnimate();

  void componentComplete() override;

  void mouseMoveEvent(QMouseEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;

  void mouseReleaseEvent(QMouseEvent *event) override;

  void mouseDoubleClickEvent(QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void touchEvent(QTouchEvent *touchEvent) override;

  void keyPressEvent(QKeyEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void focusInEvent(QFocusEvent *event) override;

  void focusOutEvent(QFocusEvent *event) override;

  void releaseResources() override;

  void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

  void itemChange(ItemChange change, const ItemChangeData &data) override;

signals:
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
  void useProgramsChanged();
  void autoAnimateChanged();
  void physicallyCorrectLightsChanged();
  void toneMappingChanged();
  void toneMappingExposureChanged();
};

}
}


#endif //THREE_PPQ_THREEDITEM_H
