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
#include <threepp/quick/scene/Scene.h>

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

public:

  FramebufferObjectRenderer(const ThreeDItem *item,
                            three::OpenGLRenderer::Ptr renderer,
                            const std::vector<Scene *> &scenes)
     : _item(item),
       _scenes(scenes),
       _renderer(renderer)
  {
    switch(item->shadowType()) {
      case Three::PCF:
        _renderer->setShadowMapType(three::ShadowMapType::PCF);
        break;
      case Three::PCFSoft:
        _renderer->setShadowMapType(three::ShadowMapType::PCFSoft);
        break;
      default:
        _renderer->setShadowMapType(three::ShadowMapType::NoShadow);
    }
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
    for(auto &scene : _scenes) {
      scene->camera()->setAspect(_item->width() / _item->height());
      scene->camera()->updateProjectionMatrix();
    }
    _target->setSize(_item->width(), _item->height());
    _renderer->setSize(_item->width(), _item->height(), setViewport);
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
  QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

ThreeDItem::~ThreeDItem()
{
}

void ThreeDItem::render(Scene *scene, Camera *camera, QJSValue prepare)
{
  _renderGroups.emplace_back(scene->scene(), camera->camera(), prepare);
}

void ThreeDItem::clear()
{
  if(_renderer) _renderer->clear();
}

void ThreeDItem::setShadowType(Three::ShadowType type) {
  if(_shadowType != type) {
    _shadowType = type;
    emit shadowTypeChanged();
  }
}

void ThreeDItem::setFaceCulling(Three::CullFace faceCulling)
{
  if(_faceCulling != faceCulling) {
    _faceCulling = faceCulling;
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

void ThreeDItem::addController(Controller *controller)
{
  controller->setItem(this);
  _controllers.push_back(controller);
}

void ThreeDItem::removeController(Controller *controller)
{
  auto found = std::find(_controllers.begin(), _controllers.end(), controller);
  if(found != _controllers.end()) _controllers.erase(found);
}

QQuickFramebufferObject::Renderer *ThreeDItem::createRenderer() const
{
  return new FramebufferObjectRenderer(this, _renderer, _scenes);
}

void ThreeDItem::mouseMoveEvent(QMouseEvent *event)
{
  for (auto contrl : _controllers) {
    if (contrl->handleMouseMoved(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::mousePressEvent(QMouseEvent *event)
{
  for (auto contrl : _controllers) {
    if (contrl->handleMousePressed(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::mouseReleaseEvent(QMouseEvent *event) {
  for (auto contrl : _controllers) {
    if (contrl->handleMouseReleased(event)) {
      update();
      return;
    }
  }
}

void ThreeDItem::mouseDoubleClickEvent(QMouseEvent *event) {
  update();
}

void ThreeDItem::wheelEvent(QWheelEvent *event) {
  for (auto contrl : _controllers) {
    if (contrl->handleMouseWheel(event)) {
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

  //window()->screen()->devicePixelRatio() QML's sizes are already adjusted
  _renderer = OpenGLRenderer::make(width(), height(), window()->screen()->devicePixelRatio());

  for(const auto &object : _objects) {
    object->addTo(this);
  }

  if(_animateFunc.isCallable()) {
    if(execAnimate()) {
      //no need to start timer if script fails on first call
      _timer = new QTimer(this);
      connect(_timer, &QTimer::timeout, this, &ThreeDItem::execAnimate, Qt::QueuedConnection);
      _timer->start(15);
    }
  }
  else if(_animateFunc.toBool()) {
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &QQuickItem::update);
    _timer->start(15);
  }
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

ThreeDItem *ThreeDItem::threeDItem()
{
  return this;
}

void ThreeDItem::addScene(Scene *scene)
{
  _scenes.push_back(scene);
  QObject::connect(scene, &Scene::sceneChanged, this, &QQuickItem::update);
}

}
}

#include "ThreeDItem.moc"