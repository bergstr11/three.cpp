//
// Created by byter on 2/9/18.
//

#include "ThreeDItem.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include <QQuickWindow>
#include <QScreen>
#include <threepp/quick/scene/Scene.h>
#include <threepp/renderers/OpenGLRenderer.h>

namespace three {
namespace quick {

class FramebufferObjectRenderer : public QObject, public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
Q_OBJECT

  const std::vector<Scene *> &_scenes;
  three::OpenGLRenderer::Ptr _renderer;
  three::Renderer::Target::Ptr _target;

  const ThreeDItem *const _item;
  QOpenGLFramebufferObject *_fbo = nullptr;

public:

  explicit FramebufferObjectRenderer(const ThreeDItem *item,
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
    _renderer->initContext();
  }

  ~FramebufferObjectRenderer() override = default;

  void synchronize(QQuickFramebufferObject *item) override
  {
  }

  void render() override
  {
    updateGeometry();

    for(auto it = _scenes.begin(); it != _scenes.end(); it++) {
      _renderer->render((*it)->scene(), (*it)->camera(), _target, it == _scenes.begin());
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

  void updateGeometry()
  {
    for(auto &scene : _scenes) {
      scene->camera()->setAspect(_item->width() / _item->height());
      scene->camera()->updateProjectionMatrix();
    }
    _renderer->setSize(_item->width(), _item->height());
  }
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

void ThreeDItem::setSamples(unsigned samples)
{
  if(_samples != samples) {
    _samples = samples;
    emit samplesChanged();
  }
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

void ThreeDItem::componentComplete()
{
  QQuickItem::componentComplete();

  _renderer = OpenGLRenderer::make(width(), height(), window()->screen()->devicePixelRatio());

  for(const auto &object : _objects) {
    object->addTo(this);
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

void ThreeDItem::addScene(Scene *scene)
{
  _scenes.push_back(scene);
  QObject::connect(scene, &Scene::sceneChanged, this, &QQuickItem::update);
}

}
}

#include "ThreeDItem.moc"