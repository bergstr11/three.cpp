//
// Created by cse on 5/12/17.
//

#include <functional>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QThread>
#include <QScreen>

#include "ThreeDScene.h"

#include <renderers/OpenGLRenderer.h>
#include <quick/objects/Plane.h>
#include <quick/objects/Box.h>
#include <quick/objects/Sphere.h>
#include <quick/objects/Axes.h>
#include <quick/materials/MeshBasicMaterial.h>
#include <quick/cameras/PerspectiveCamera.h>

namespace three {
namespace quick {

using namespace std;

class FramebufferObjectRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
  QColor m_background;

  three::Scene::Ptr _scene;
  three::Camera::Ptr _camera;
  three::OpenGLRenderer::Ptr _renderer;
  three::Renderer::Target::Ptr _target;

  const ThreeDScene *const _item;
  QOpenGLFramebufferObject *_framebufferObject = nullptr;

public:

  explicit FramebufferObjectRenderer(const ThreeDScene *item, three::Scene::Ptr scene, three::Camera::Ptr camera)
     : _item(item),
       _scene(scene),
       _camera(camera),
       _renderer(OpenGLRenderer::make(
          QOpenGLContext::currentContext(),
          item->width(), item->height(),
          item->window()->screen()->devicePixelRatio()))
  {
    _renderer->setSize(item->width(), item->height());

    _camera->lookAt(_scene->position());
  }

  ~FramebufferObjectRenderer() override = default;

  void synchronize(QQuickFramebufferObject *_item) override
  {
    auto *item = static_cast<ThreeDScene *>(_item);
  }

  void render() override
  {
    _renderer->render(_scene, _camera, _target);
    _item->window()->resetOpenGLState();
  }

  QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
  {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setMipmap(false);
    format.setInternalTextureFormat(GL_RGBA);

    _framebufferObject = new QOpenGLFramebufferObject(size, format);

    _target = OpenGLRenderer::makeExternalTarget(
       _framebufferObject->handle(), _framebufferObject->texture(), _item->width(), _item->height());

    return _framebufferObject;
  }
};

ThreeDScene::ThreeDScene(QQuickItem *parent) : QQuickFramebufferObject(parent)
{
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemAcceptsInputMethod);

  setMirrorVertically(true);
}

ThreeDScene::~ThreeDScene()
{
}

QQuickFramebufferObject::Renderer *ThreeDScene::createRenderer() const
{
  return new FramebufferObjectRenderer(this, _scene, _camera);
}

void ThreeDScene::setBackground(const QColor &background) {
  if (_background != background) {
    _background = background;
    emit backgroundChanged();
  }
}

void ThreeDScene::setName(const QString &name) {
  if (_name != name) {
    _name = name;
    emit nameChanged();
  }
}

void ThreeDScene::setCamera(Camera *camera)
{
  if(_quickCamera != camera) {
    if(_quickCamera) {
      _quickCamera->deleteLater();
    }
    _quickCamera = camera;
    emit cameraChanged();
  }
}

void ThreeDScene::append_object(QQmlListProperty<ThreeDObject> *list, ThreeDObject *obj)
{
  ThreeDScene *scene = qobject_cast<ThreeDScene *>(list->object);
  if (scene) scene->_objects.append(obj);
}
int ThreeDScene::count_objects(QQmlListProperty<ThreeDObject> *list)
{
  ThreeDScene *scene = qobject_cast<ThreeDScene *>(list->object);
  return scene ? scene->_objects.size() : 0;
}
ThreeDObject *ThreeDScene::object_at(QQmlListProperty<ThreeDObject> *list, int index)
{
  ThreeDScene *scene = qobject_cast<ThreeDScene *>(list->object);
  return scene ? scene->_objects.at(index) : nullptr;
}
void ThreeDScene::clear_objects(QQmlListProperty<ThreeDObject> *list)
{
  ThreeDScene *scene = qobject_cast<ThreeDScene *>(list->object);
  if(scene) scene->_objects.clear();
}

QQmlListProperty<ThreeDObject> ThreeDScene::objects()
{
  return QQmlListProperty<ThreeDObject>(this, nullptr,
                                        &ThreeDScene::append_object,
                                        &ThreeDScene::count_objects,
                                        &ThreeDScene::object_at,
                                        &ThreeDScene::clear_objects);
}

void ThreeDScene::init()
{
  qmlRegisterType<three::quick::ThreeDScene>("three.quick", 1, 0, "Scene");
  qmlRegisterUncreatableType<three::quick::ThreeDObject>("three.quick", 1, 0, "ThreeDObject", "abstract class");
  qmlRegisterUncreatableType<three::quick::Material>("three.quick", 1, 0, "Material", "abstract class");
  qmlRegisterUncreatableType<three::quick::Camera>("three.quick", 1, 0, "Camera", "abstract class");
  qmlRegisterType<three::quick::Axes>("three.quick", 1, 0, "Axes");
  qmlRegisterType<three::quick::Box>("three.quick", 1, 0, "Box");
  qmlRegisterType<three::quick::Plane>("three.quick", 1, 0, "Plane");
  qmlRegisterType<three::quick::Sphere>("three.quick", 1, 0, "Sphere");
  qmlRegisterType<three::quick::MeshBasicMaterial>("three.quick", 1, 0, "MeshBasicMaterial");
  qmlRegisterType<three::quick::PerspectiveCamera>("three.quick", 1, 0, "PerspectiveCamera");
}

void ThreeDScene::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
  QQuickFramebufferObject::geometryChanged(newGeometry, oldGeometry);
}

void ThreeDScene::mouseMoveEvent(QMouseEvent *event) {
  update();
}

void ThreeDScene::mousePressEvent(QMouseEvent *event) {
  update();
}

void ThreeDScene::mouseReleaseEvent(QMouseEvent *event) {
  update();
}

void ThreeDScene::mouseDoubleClickEvent(QMouseEvent *event) {
  update();
}

void ThreeDScene::wheelEvent(QWheelEvent *event) {
  update();
}


void ThreeDScene::keyPressEvent(QKeyEvent *event) {
  update();
}

void ThreeDScene::keyReleaseEvent(QKeyEvent *event) {
  update();
}

void ThreeDScene::focusInEvent(QFocusEvent *event) {
  forceActiveFocus();
}

void ThreeDScene::focusOutEvent(QFocusEvent *event) {
  update();
}

void ThreeDScene::releaseResources() {
  QQuickFramebufferObject::releaseResources();
}

void ThreeDScene::componentComplete()
{
  QQuickItem::componentComplete();

  _scene = SceneT<Color>::make(_name.toStdString(),
                               Color(_background.redF(), _background.greenF(), _background.blueF()));

  for(auto &object :_objects) {
    auto obj = object->create();
    if(obj) _scene->add(obj);
  }
  _camera = _quickCamera->create();
}

}
}
