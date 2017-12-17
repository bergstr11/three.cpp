//
// Created by cse on 5/12/17.
//

#include <functional>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QThread>
#include <QScreen>

#include "ThreeDItem.h"
#include "ThreeDInteractor.h"

#include <renderers/OpenGLRenderer.h>
#include <objects/Mesh.h>
#include <scene/Scene.h>
#include <camera/PerspectiveCamera.h>
#include <material/MeshLambertMaterial.h>
#include <geometry/Box.h>
#include <geometry/Plane.h>
#include <geometry/Sphere.h>
#include <helper/AxesHelper.h>
#include <light/SpotLight.h>

namespace lo {
namespace ui {
namespace quick {

using namespace std;
using namespace three;
using namespace three::geometry;
using namespace three::helper;

class FramebufferObjectRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
  QColor m_background;

  three::Scene::Ptr _scene;
  three::PerspectiveCamera::Ptr _camera;
  three::OpenGLRenderer::Ptr _renderer;
  three::Renderer::Target::Ptr _target;

  const ThreeDItem *const _item;

public:

  explicit FramebufferObjectRenderer(const ThreeDItem *item)
     : _item(item),
       _scene(SceneT<Color>::make("scene", Color(0.4f, 0.4f, 0.6f))),
       _camera(PerspectiveCamera::make(45, item->width() / item->height(), 0.1, 1000)),
       _renderer(OpenGLRenderer::make(
          QOpenGLContext::currentContext(),
          item->width(), item->height(),
          item->window()->screen()->devicePixelRatio()))
  {
    _renderer->setSize(item->width(), item->height());
    _renderer->setShadowsEnabled(true);

#if 1
    //Axes
    AxesHelper::Ptr axes = AxesHelper::make("axis", 20);

    _scene->add(axes);

    //Plane
    Plane::Ptr planeGeometry = Plane::make(60, 20, 1, 1);
    MeshLambertMaterial::Ptr planeMaterial = MeshLambertMaterial::make();
    planeMaterial->color = Color(0xcccccc);

    Mesh::Ptr plane = Mesh_T<Plane, MeshLambertMaterial>::make("plane", planeGeometry, planeMaterial);
    plane->rotation().setX(-0.5f * (float) M_PI);
    plane->position().set(15, 0, 0);
    plane->receiveShadow = true;

    _scene->add(plane);

    //Cube
    Box::Ptr cubeGeometry = Box::make(4, 4, 4);
    MeshLambertMaterial::Ptr cubeMaterial = MeshLambertMaterial::make();
    cubeMaterial->color = Color(0xff0000);

    Mesh::Ptr cube = Mesh_T<Box, MeshLambertMaterial>::make("cube", cubeGeometry, cubeMaterial);
    cube->position().set(-4, 3, 0);
    cube->castShadow = true;

    _scene->add(cube);
#endif
    //Sphere
    Sphere::Ptr sphereGeometry = Sphere::make(4, 20, 20);
    MeshLambertMaterial::Ptr sphereMaterial = MeshLambertMaterial::make();
    sphereMaterial->color = Color(0x7777ff);

    Mesh::Ptr sphere = Mesh_T<Sphere, MeshLambertMaterial>::make("sphere", sphereGeometry, sphereMaterial);
    sphere->position().set(20, 4, 2);
    cube->castShadow = true;

    _scene->add(sphere);

    //Light
    SpotLight::Ptr spotLight = SpotLight::make(_scene, Color(0xffffff));
    spotLight->position().set(-40, 60, -10);
    spotLight->castShadow = true;
    _scene->add(spotLight);

    _camera->position().set(-30, 40, 30);
    _camera->lookAt(_scene->position());
  }

  ~FramebufferObjectRenderer() override = default;

  void synchronize(QQuickFramebufferObject *_item) override
  {
    auto *item = static_cast<ThreeDItem *>(_item);
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

    QOpenGLFramebufferObject *fbo = new QOpenGLFramebufferObject(size, format);

    _renderer->setExternalDefaults(fbo->handle(), GL_TEXTURE0);
    _target = OpenGLRenderer::makeExternalTarget(
       fbo->handle(), fbo->texture(), _item->width(), _item->height());

    return fbo;
  }
};

ThreeDItem::ThreeDItem(QQuickItem *parent)
   : QQuickFramebufferObject(parent), m_model(nullptr), m_interactor(new ThreeDInteractor())
{
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemAcceptsInputMethod);

  setMirrorVertically(true);

  //m_loader = new ModelLoader();
  //connect(m_loader, &ModelLoader::modelLoaded, this, &ThreeDItem::modelLoaded, Qt::QueuedConnection);
}

ThreeDItem::~ThreeDItem()
{
  delete m_interactor;
  //delete m_loader;
}

QQuickFramebufferObject::Renderer *ThreeDItem::createRenderer() const
{
  return new FramebufferObjectRenderer(this);
}

void ThreeDItem::setData(QString data) {
  if (data != m_data) {
    m_data = data;

    //m_loader->asynchLoad(QFileInfo(m_data));
  }
}

void ThreeDItem::modelLoaded()
{
  //m_model = m_loader->getLoaded();
  //m_pendingModel = true;
  update();
  emit dataChanged();
}


void ThreeDItem::setBackground(QColor background) {
  if (m_background != background) {
    m_background = background;
    emit backgroundChanged();
  }
}

const shared_ptr<Scene> ThreeDItem::pendingModel()
{
  //if (m_pendingModel) {
  //  m_pendingModel = false;
  //  return m_model;
  //}
  return nullptr;
}

void ThreeDItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
  QQuickFramebufferObject::geometryChanged(newGeometry, oldGeometry);
}

void ThreeDItem::mouseMoveEvent(QMouseEvent *event) {
  m_interactor->SetEvent(event);
  update();
}

void ThreeDItem::mousePressEvent(QMouseEvent *event) {
  m_interactor->SetEvent(event);
  update();
}

void ThreeDItem::mouseReleaseEvent(QMouseEvent *event) {
  m_interactor->SetEvent(event);
  update();
}

void ThreeDItem::mouseDoubleClickEvent(QMouseEvent *event) {
  m_interactor->SetEvent(event);
  update();
}

void ThreeDItem::wheelEvent(QWheelEvent *event) {
  m_interactor->SetEvent(event);
  update();
}


void ThreeDItem::keyPressEvent(QKeyEvent *event) {
  m_interactor->SetEvent(event);
  update();
}

void ThreeDItem::keyReleaseEvent(QKeyEvent *event) {
  m_interactor->SetEvent(event);
  update();
}

void ThreeDItem::focusInEvent(QFocusEvent *event) {
  m_interactor->SetEvent(event);
  forceActiveFocus();
}

void ThreeDItem::focusOutEvent(QFocusEvent *event) {
  m_interactor->SetEvent(event);
  update();
}

void ThreeDItem::releaseResources() {
  QQuickFramebufferObject::releaseResources();
}
}
}
}
