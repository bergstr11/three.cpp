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

#include <renderers/OpenGLRenderer.h>
#include <objects/Mesh.h>
#include <scene/Scene.h>
#include <camera/PerspectiveCamera.h>
#include <material/MeshLambertMaterial.h>
#include <material/MeshPhongMaterial.h>
#include <geometry/Box.h>
#include <geometry/Plane.h>
#include <geometry/Sphere.h>
#include <helper/Axes.h>
#include <helper/SpotLight.h>
#include <light/AmbientLight.h>

namespace lo {
namespace ui {
namespace quick {

using namespace std;
using namespace three;
using namespace three::geometry;

class FramebufferObjectRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
  QColor m_background;

  Scene::Ptr _scene;
  PerspectiveCamera::Ptr _camera;
  OpenGLRenderer::Ptr _renderer;
  three::Renderer::Target::Ptr _target;
  helper::SpotLight::Ptr _lightHelper;

  const ThreeDItem *const _item;

public:

  explicit FramebufferObjectRenderer(const ThreeDItem *item)
     : _item(item),
       _scene(SceneT<Color>::make("scene", Color(0.4f, 0.4f, 0.6f))),
       _camera(PerspectiveCamera::make(35, item->width() / item->height(), 1, 1000)),
       _renderer(OpenGLRenderer::make(
          QOpenGLContext::currentContext(),
          item->width(), item->height(),
          item->window()->screen()->devicePixelRatio()))
  {
    _renderer->setSize(item->width(), item->height());
    _renderer->setShadowMapType(ShadowMapType::PCFSoft);

    _camera->position().set( 65, 8, - 10 );
    _camera->lookAt(_scene->position());

    auto ambient = AmbientLight::make( 0xffffff, 0.1 );
    _scene->add( ambient );

    auto spotLight = SpotLight::make(_scene, 0xffffff, 1, 200, M_PI/4, 0.05, 2);
    spotLight->position().set( 15, 40, 35 );

    spotLight->castShadow = true;
    spotLight->shadow()->mapSize().set(1024, 1024);
    spotLight->shadow()->camera()->setNearFar(10, 200);
    _scene->add( spotLight );

    _lightHelper = helper::SpotLight::make("spottracker", spotLight );
    _scene->add( _lightHelper );

    //shadowCameraHelper = new THREE.CameraHelper( spotLight.shadow.camera );
    //scene.add( shadowCameraHelper );

    _scene->add( helper::Axes::make("axes", 10));

    auto material = MeshPhongMaterial::make(0x808080, true);
    auto geometry = Box::make( 2000, 1, 2000 );

    auto mesh = MeshT<Box, MeshPhongMaterial>::make( geometry, material );
    mesh->position().set( 0, - 1, 0 );
    mesh->receiveShadow = true;
    _scene->add( mesh );

    material = MeshPhongMaterial::make(0x4080ff, true);
    geometry = geometry::Box::make( 3, 1, 2 );

    mesh = MeshT<Box, MeshPhongMaterial>::make( geometry, material );
    mesh->position().set( 40, 2, 0 );
    mesh->castShadow = true;
    _scene->add( mesh );
  }

  ~FramebufferObjectRenderer() override = default;

  void synchronize(QQuickFramebufferObject *_item) override
  {
    auto *item = static_cast<ThreeDItem *>(_item);
  }

  void render() override
  {
    _lightHelper->update();
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

    _target = OpenGLRenderer::makeExternalTarget(
       fbo->handle(), fbo->texture(), _item->width(), _item->height());

    return fbo;
  }
};

ThreeDItem::ThreeDItem(QQuickItem *parent)
   : QQuickFramebufferObject(parent), m_model(nullptr)
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
  update();
}

void ThreeDItem::mousePressEvent(QMouseEvent *event) {
  update();
}

void ThreeDItem::mouseReleaseEvent(QMouseEvent *event) {
  update();
}

void ThreeDItem::mouseDoubleClickEvent(QMouseEvent *event) {
  update();
}

void ThreeDItem::wheelEvent(QWheelEvent *event) {
  update();
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
}
}
}
