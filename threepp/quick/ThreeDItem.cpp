//
// Created by byter on 2/9/18.
//

#include "ThreeDItem.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QScreen>
#include <QTimer>
#include <QGuiApplication>
#include <QStyleHints>
#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/cameras/Camera.h>

namespace three {
namespace quick {

inline bool handle_jserror(const QJSValue &jsValue)
{
  if (jsValue.isError()) {
    qWarning() << "javascript error at line"
               << jsValue.property("lineNumber").toInt()
               << ":" << jsValue.toString();
    return true;
  }
  return false;
}

class FramebufferObjectRenderer : public QObject, public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
Q_OBJECT
  Q_PROPERTY(QRect viewport READ viewport WRITE setViewport NOTIFY viewportChanged)

  const std::vector<Scene *> &_scenes;
  three::OpenGLRenderer::Ptr _renderer;
  three::Renderer::Target::Ptr _target;

  const ThreeDItem *const _item;
  QOpenGLFramebufferObject *_fbo = nullptr;
  std::vector<ThreeDItem::RenderGroup> _renderGroups;

  QJSValue _jsInstance;

  Signal<void(OpenGLRenderer::Ptr, three::Renderer::Target::Ptr)> *_onRendered = nullptr;

public:

  FramebufferObjectRenderer(const ThreeDItem *item,
                            three::OpenGLRenderer::Ptr renderer,
                            const std::vector<Scene *> &scenes)
     : _item(item),
       _scenes(scenes),
       _renderer(renderer)
  {
    switch(item->shadowMap()->type()) {
      case Three::Basic:
        _renderer->setShadowMapType(three::ShadowMapType::Basic);
        break;
      case Three::PCF:
        _renderer->setShadowMapType(three::ShadowMapType::PCF);
        break;
      case Three::PCFSoft:
        _renderer->setShadowMapType(three::ShadowMapType::PCFSoft);
        break;
      default:
        _renderer->setShadowMapType(three::ShadowMapType::NoShadow);
    }
    _renderer->setShadowMapAuto(_item->shadowMap()->autoUpdate());

    _renderer->autoClear = _item->_autoClear;
    _renderer->antialias = _item->_antialias;
    _renderer->initContext();

    _jsInstance = qmlEngine(_item)->newQObject(this);
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
  }

  ~FramebufferObjectRenderer() override {
    _renderGroups.clear();
    _jsInstance = QJSValue::NullValue;
  }

  void synchronize(QQuickFramebufferObject *item) override
  {
    ThreeDItem *threeD = reinterpret_cast<ThreeDItem *>(item);
    _renderGroups = threeD->_renderGroups;
    _onRendered = &threeD->onRendered;
    if(!threeD->_viewport.isNull())
      _target->setViewport(threeD->_viewport.x(), threeD->_viewport.y(),
                           threeD->_viewport.width(), threeD->_viewport.height());
    threeD->_renderGroups.clear();
  }

  void render() override
  {
    std::lock_guard<std::mutex> lock(_renderer->mutex);

    if(_item->_autoRender && _renderGroups.empty()) {
      updateGeometry(_item->_viewport.isNull());

      for(auto it = _scenes.begin(); it != _scenes.end(); it++) {
        _renderer->render((*it)->scene(), (*it)->camera(), _target, it == _scenes.begin());
        _onRendered->emitSignal(_renderer, _target);
      }
    }
    else {
      _renderer->setSize(_item->width(), _item->height(), false);
      for(size_t i=0, l=_renderGroups.size(); i<l; i++) {
        auto group = _renderGroups.back();
        _renderGroups.pop_back();

        if(group.prepare.isCallable()) {
          handle_jserror(group.prepare.callWithInstance(_jsInstance));
        }
        _renderer->render(group.scene, group.camera, _target, false);
      }
    }

    _item->window()->resetOpenGLState();
  }

  QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
  {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setMipmap(true);
    format.setSamples(_item->_samples);
    format.setInternalTextureFormat(GL_RGBA);

    _fbo = new QOpenGLFramebufferObject(size, format);

    _target = OpenGLRenderer::makeExternalTarget(
       _fbo->handle(), _fbo->texture(), _item->width(), _item->height(),
       (CullFace)_item->faceCulling(), (FrontFaceDirection)_item->faceDirection());

    return _fbo;
  }

  void updateGeometry(bool setViewport)
  {
    _target->setSize(_item->width(), _item->height());
    _renderer->setSize(_item->width(), _item->height(), setViewport);

    for(auto &scene : _scenes) {
      scene->camera()->setAspect(_item->width() / _item->height());
      scene->camera()->updateProjectionMatrix();
    }
  }

  QRect viewport() {
    return QRect(_target->viewport().x(), _target->viewport().y(), _target->viewport().z(), _target->viewport().w());
  }

  void setViewport(const QRect &viewport) {
    unsigned y = ((unsigned)_item->height() - viewport.height() - viewport.y()); //flip vertically
    _target->setViewport(viewport.x(), y, viewport.width(), viewport.height());
  }
signals:
  void viewportChanged();
};

ThreeDItem::ThreeDItem(QQuickItem *parent) : QQuickFramebufferObject(parent)
{
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemAcceptsInputMethod);

  setMirrorVertically(true);
}

ThreeDItem::~ThreeDItem()
{
}

void ThreeDItem::lockWhile(std::function<void()> func)
{
  std::lock_guard<std::mutex> lock(_renderer->mutex);
  func();
}

void ThreeDItem::render(Scene *scene, Camera *camera, QJSValue prepare)
{
  _renderGroups.emplace_back(scene->scene(), camera->camera(), prepare);
}

void ThreeDItem::runAnimation(bool animate)
{
  if(_animateTimer) {
    if(animate && !_animateTimer->isActive())
      _animateTimer->start();
    else if(!animate && _animateTimer->isActive())
      _animateTimer->stop();
  }
}

void ThreeDItem::clear()
{
  if(_renderer) _renderer->clear();
}

void ShadowMap::setType(Three::ShadowType type) {
  if(_shadowType != type) {
    _shadowType = type;
    if(_renderer) _renderer->setShadowMapType((three::ShadowMapType)type);
    emit typeChanged();
  }
}

void ShadowMap::setAutoUpdate(bool autoUpdate)
{
  if(_autoUpdate != autoUpdate) {
    _autoUpdate = autoUpdate;
    if(_renderer) _renderer->setShadowMapAuto(autoUpdate);
    emit autoUpdateChanged();
  }
}

void ShadowMap::setNeedsUpdate(bool needsUpdate)
{
  if(_needsUpdate != needsUpdate) {
    _needsUpdate = needsUpdate;
    if(_renderer) _renderer->updateShadows();
    emit autoUpdateChanged();
  }
}

void ThreeDItem::setFaceCulling(Three::CullFace faceCulling)
{
  if(_faceCulling != faceCulling) {
    _faceCulling = faceCulling;
    if(_renderer) _renderer->setFaceCulling((three::CullFace)_faceCulling);
    emit faceCullingChanged();
  }
}

void ThreeDItem::setFaceDirection(Three::FrontFaceDirection faceDirection)
{
  if(_faceDirection != faceDirection) {
    _faceDirection = faceDirection;
    emit faceDirectionChanged();
  }
}

void ThreeDItem::setAutoClear(bool autoClear)
{
  if(_autoClear != autoClear) {
    _autoClear = autoClear;
    emit autoClearChanged();
  }
}

void ThreeDItem::setAutoRender(bool autoRender)
{
  if(_autoRender != autoRender) {
    _autoRender = autoRender;
    emit autoRenderChanged();
  }
}

void ThreeDItem::setAntialias(bool antialias)
{
  if(_antialias != antialias) {
    _antialias = antialias;
    emit antialiasChanged();
  }
}

void ThreeDItem::setAutoAnimate(bool autoAnimate)
{
  if(_autoAnimate != autoAnimate) {
    _autoAnimate = autoAnimate;
    emit autoAnimateChanged();
  }
}

void ThreeDItem::setSamples(unsigned samples)
{
  if(_samples != samples) {
    _samples = samples;
    emit samplesChanged();
  }
}

void ThreeDItem::setViewport(QRect viewport)
{
  viewport.setY((int)height() - viewport.height() - viewport.y()); //flip vertically
  if(_viewport != viewport) {
    _viewport = viewport;
    emit viewportChanged();
  }
}

void ThreeDItem::setAnimate(QJSValue animate)
{
  _animateFunc = animate;
  _jsInstance = qmlEngine(this)->newQObject(this);
}

void ThreeDItem::setFps(unsigned fps)
{
  if(_fps != fps) {
    _fps = fps;
    emit fpsChanged();
  }
}

void ThreeDItem::setUsePrograms(ThreeDItem *item)
{
  if(_usePrograms != item) {
    if(_usePrograms) throw std::logic_error("usePrograms con only be set once");

    _usePrograms = item;
    if(_renderer) _renderer->usePrograms(item->_renderer);

    emit useProgramsChanged();
  }
}

void ThreeDItem::addInteractor(Interactor *interactor)
{
  _interactors.push_back(interactor);
}

void Interactor::setItem(ThreeDItem *item)
{
  if(_item) _item->removeInteractor(this);
  _item = item;
  _item->addInteractor(this);

  _clickedTimer.setSingleShot(true);
  QObject::connect(&_clickedTimer, &QTimer::timeout, [this]() {
    handleMouseClicked(&_clicked);
  });
}

bool Interactor::mouseReleased(QMouseEvent *event) {
  return _enabled && !_unifyClicked ? handleMouseReleased(event) : false;
}

bool Interactor::mousePressed(QMouseEvent *event)
{
  if(_enabled) {
    if(!_unifyClicked) return handleMousePressed(event);

    if(!_clickedTimer.isActive()) {
      _clicked = *event;
      _clickedTimer.start(QGuiApplication::styleHints()->mouseDoubleClickInterval() + 20);
    }
  }
  return false;
}

bool Interactor::mouseDoubleClicked(QMouseEvent *event)
{
  if(_enabled) {
    _clickedTimer.stop();
    return handleMouseDoubleClicked(event);
  }
  return false;
}

void ThreeDItem::removeInteractor(Interactor *controller)
{
  auto found = std::find(_interactors.begin(), _interactors.end(), controller);
  if(found != _interactors.end()) _interactors.erase(found);
}

QQuickFramebufferObject::Renderer *ThreeDItem::createRenderer() const
{
  return new FramebufferObjectRenderer(this, _renderer, _scenes);
}

void ThreeDItem::mouseMoveEvent(QMouseEvent *event)
{
  for (auto contrl : _interactors) {
    if (contrl->mouseMoved(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::mousePressEvent(QMouseEvent *event)
{
  for (auto contrl : _interactors) {
    if (contrl->mousePressed(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::mouseReleaseEvent(QMouseEvent *event)
{
  for (auto contrl : _interactors) {
    if (contrl->mouseReleased(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::mouseDoubleClickEvent(QMouseEvent *event)
{
  for (auto contrl : _interactors) {
    if (contrl->mouseDoubleClicked(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::wheelEvent(QWheelEvent *event)
{
  for (auto contrl : _interactors) {
    if (contrl->mouseWheel(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
  QQuickFramebufferObject::geometryChanged(newGeometry, oldGeometry);
  emit geometryChanged();
}

void ThreeDItem::keyPressEvent(QKeyEvent *event) {
  update();
}

void ThreeDItem::keyReleaseEvent(QKeyEvent *event) {
  update();
}

void ThreeDItem::focusInEvent(QFocusEvent *event) {
  forceActiveFocus();
}

void ThreeDItem::focusOutEvent(QFocusEvent *event) {
  update();
}

void ThreeDItem::releaseResources() {
  QQuickFramebufferObject::releaseResources();
}

bool ThreeDItem::execAnimate()
{
  std::unique_lock<std::mutex> lock(_renderer->mutex, std::try_to_lock);
  if(lock) {
    QJSValue jsRes = _animateFunc.callWithInstance(_jsInstance);
    if(handle_jserror(jsRes)) return false;

    update();
  }

  return true;
}

void ThreeDItem::componentComplete()
{
  QQuickItem::componentComplete();

  _renderer = OpenGLRenderer::make(width(), height(), window()->screen()->devicePixelRatio());
  if(_usePrograms) _renderer->usePrograms(_usePrograms->_renderer);
  _shadowMap.setRenderer(_renderer);

  for(const auto &object : _objects) {
    Scene *scene = dynamic_cast<Scene *>(object);
    if(scene) scene->setItem(this);
  }
  for(const auto &object : _objects) {
    Scene *scene = dynamic_cast<Scene *>(object);
    if(!scene) object->setItem(this);
  }

  if(_animateFunc.isCallable()) {
    _animateTimer = new QTimer(this);
    _animateTimer->setInterval(1000.0f / _fps);
    connect(_animateTimer, &QTimer::timeout, this, &ThreeDItem::execAnimate, Qt::QueuedConnection);

    if(_autoAnimate) _animateTimer->start(1000.0f / _fps);
  }
  else if(_animateFunc.toBool()) {
    _animateTimer = new QTimer(this);
    _animateTimer->setInterval(1000.0f / _fps);
    connect(_animateTimer, &QTimer::timeout, this, &QQuickItem::update);

    if(_autoAnimate) _animateTimer->start();
  }
}

void ThreeDItem::itemChange(ItemChange change, const ItemChangeData &data)
{
  if(_animateTimer) {
    enum {on, off, skip} state = skip;
    switch(change) {
      case QQuickItem::ItemVisibleHasChanged:
        state = data.boolValue && isEnabled() ? on : off;
        break;
      case QQuickItem::ItemEnabledHasChanged:
        state = data.boolValue && isVisible() ? on : off;
        break;
    }
    if(state != skip) {
      if(state == on && !_animateTimer->isActive()) {
        _animateTimer->start(1000.0f / _fps);
      }
      else if(_animateTimer->isActive()) {
        _animateTimer->stop();
      }
    }
  }
  QQuickItem::itemChange(change, data);
}

void ThreeDItem::append_object(QQmlListProperty<ThreeQObjectRoot> *list, ThreeQObjectRoot *obj)
{
  ThreeDItem *item = qobject_cast<ThreeDItem *>(list->object);
  if (item) item->_objects.append(obj);
}
int ThreeDItem::count_objects(QQmlListProperty<ThreeQObjectRoot> *list)
{
  ThreeDItem *item = qobject_cast<ThreeDItem *>(list->object);
  return item ? item->_objects.size() : 0;
}
ThreeQObjectRoot *ThreeDItem::object_at(QQmlListProperty<ThreeQObjectRoot> *list, int index)
{
  ThreeDItem *item = qobject_cast<ThreeDItem *>(list->object);
  return item ? item->_objects.at(index) : nullptr;
}
void ThreeDItem::clear_objects(QQmlListProperty<ThreeQObjectRoot> *list)
{
  ThreeDItem *item = qobject_cast<ThreeDItem *>(list->object);
  if(item) item->_objects.clear();
}

QQmlListProperty<ThreeQObjectRoot> ThreeDItem::objects()
{
  return QQmlListProperty<ThreeQObjectRoot>(this, nullptr,
                                            &ThreeDItem::append_object,
                                            &ThreeDItem::count_objects,
                                            &ThreeDItem::object_at,
                                            &ThreeDItem::clear_objects);
}

void ThreeDItem::addScene(Scene *scene)
{
  _scenes.push_back(scene);
}

}
}

#include "ThreeDItem.moc"
