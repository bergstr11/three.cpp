//
// Created by byter on 12/29/17.
//

#include <QMetaType>
#include <math/Euler.h>

Q_DECLARE_METATYPE(three::math::Euler);
#include "Three.h"

#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include <QQuickWindow>
#include <QScreen>
#include <renderers/OpenGLRenderer.h>
#include "quick/scene/Scene.h"
#include "quick/scene/Model.h"
#include "elements/LightShadow.h"
#include "objects/AmbientLight.h"
#include "objects/SpotLight.h"
#include "objects/Axes.h"
#include "objects/Box.h"
#include "objects/Plane.h"
#include "objects/Sphere.h"
#include "objects/Mesh.h"
#include "materials/MeshBasicMaterial.h"
#include "materials/MeshLambertMaterial.h"
#include "materials/MeshPhongMaterial.h"
#include "materials/ShaderMaterial.h"
#include "textures/Texture.h"
#include "textures/ImageTexture.h"
#include "textures/ImageCubeTexture.h"
#include "interact/OrbitController.h"

namespace three {
namespace quick {

void init()
{
  qRegisterMetaType<three::math::Euler>();
  qmlRegisterUncreatableType<three::quick::Three>("three.quick", 1, 0, "Three", "enum holder class");
  qmlRegisterType<three::quick::ThreeDItem>("three.quick", 1, 0, "ThreeD");
  qmlRegisterType<three::quick::Scene>("three.quick", 1, 0, "Scene");
  qmlRegisterType<three::quick::Model>("three.quick", 1, 0, "Model");
  qmlRegisterUncreatableType<three::quick::ThreeQObjectRoot>("three.quick", 1, 0, "ThreeQObjectRoot", "abstract class");
  qmlRegisterUncreatableType<three::quick::ThreeQObject>("three.quick", 1, 0, "ThreeQObject", "abstract class");
  qmlRegisterUncreatableType<three::quick::Material>("three.quick", 1, 0, "Material", "abstract class");
  qmlRegisterUncreatableType<three::quick::Camera>("three.quick", 1, 0, "Camera", "abstract class");
  qmlRegisterUncreatableType<three::quick::LightShadow>("three.quick", 1, 0, "LightShadow", "internal class");
  qmlRegisterUncreatableType<three::quick::FogBase>("three.quick", 1, 0, "FogBase", "abstract class");
  qmlRegisterType<three::quick::Fog>("three.quick", 1, 0, "Fog");
  qmlRegisterType<three::quick::FogExp2>("three.quick", 1, 0, "FogExp2");
  qmlRegisterType<three::quick::Axes>("three.quick", 1, 0, "Axes");
  qmlRegisterType<three::quick::Box>("three.quick", 1, 0, "Box");
  qmlRegisterType<three::quick::Plane>("three.quick", 1, 0, "Plane");
  qmlRegisterType<three::quick::Sphere>("three.quick", 1, 0, "Sphere");
  qmlRegisterType<three::quick::Mesh>("three.quick", 1, 0, "Mesh");
  qmlRegisterType<three::quick::AmbientLight>("three.quick", 1, 0, "AmbientLight");
  qmlRegisterType<three::quick::SpotLight>("three.quick", 1, 0, "SpotLight");
  qmlRegisterType<three::quick::MeshBasicMaterial>("three.quick", 1, 0, "MeshBasicMaterial");
  qmlRegisterType<three::quick::MeshLambertMaterial>("three.quick", 1, 0, "MeshLambertMaterial");
  qmlRegisterType<three::quick::MeshPhongMaterial>("three.quick", 1, 0, "MeshPhongMaterial");
  qmlRegisterType<three::quick::ShaderMaterial>("three.quick", 1, 0, "ShaderMaterial");
  qmlRegisterType<three::quick::PerspectiveCamera>("three.quick", 1, 0, "PerspectiveCamera");
  qmlRegisterUncreatableType<three::quick::Texture>("three.quick", 1, 0, "Texture", "abstract class");
  qmlRegisterType<three::quick::Image>("three.quick", 1, 0, "Image");
  qmlRegisterType<three::quick::ImageTexture>("three.quick", 1, 0, "ImageTexture");
  qmlRegisterType<three::quick::ImageCubeTexture>("three.quick", 1, 0, "ImageCubeTexture");
  qmlRegisterUncreatableType<three::quick::Controller>("three.quick", 1, 0, "Controller", "abstract class");
  qmlRegisterType<three::quick::OrbitController>("three.quick", 1, 0, "OrbitController");
}

using namespace std;

class FramebufferObjectRenderer : public QObject, public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
Q_OBJECT

  QColor m_background;

  const std::vector<Scene *> &_scenes;
  three::OpenGLRenderer::Ptr _renderer;
  three::Renderer::Target::Ptr _target;

  const ThreeDItem *const _item;
  QOpenGLFramebufferObject *_fbo = nullptr;

public:

  explicit FramebufferObjectRenderer(const ThreeDItem *item,
                                     const std::vector<Scene *> &scenes)
     : _item(item),
       _scenes(scenes),
       _renderer(OpenGLRenderer::make(
          QOpenGLContext::currentContext(),
          item->width(), item->height(),
          item->window()->screen()->devicePixelRatio()))
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
  }

  ~FramebufferObjectRenderer() override = default;

  void synchronize(QQuickFramebufferObject *_item) override
  {
  }

  void render() override
  {
    updateGeometry();

    for(auto it = _scenes.begin(); it != _scenes.end(); it++) {
      (*it)->quickCamera()->update();
    }
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

QQuickFramebufferObject::Renderer *ThreeDItem::createRenderer() const
{
  return new FramebufferObjectRenderer(this, _scenes);
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

void ThreeDItem::addMaterial(Material *material)
{

}

void ThreeDItem::addTexture(Texture *texture)
{

}

void ThreeDItem::addScene(Scene *scene)
{
  _scenes.push_back(scene);
}

}
}

#include "Three.moc"
