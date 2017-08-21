//
// Created by cse on 5/12/17.
//

#include <functional>

#include <QOpenGLFunctions_3_0>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QThread>

#include "ThreeDItem.h"
#include "ThreeDInteractor.h"

#include <OpenGLRenderer.h>
#include <objects/Mesh.h>
#include <scene/Scene.h>
#include <camera/PerspectiveCamera.h>
#include <material/MeshBasicMaterial.h>
#include <geometry/Box.h>
#include <geometry/Plane.h>
#include <helper/AxisHelper.h>
#include "bak/modelloader.h"

namespace lo {
namespace ui {
namespace quick {

using namespace std;
using namespace three;
using namespace three::geometry;
using namespace three::helper;

class FramebufferObjectRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions_3_0
{
  QColor m_background;

  three::Scene m_scene;
  three::PerspectiveCamera _camera;
  three::OpenGLRenderer _renderer;

  const ThreeDItem *const _item;
  QOpenGLFramebufferObject *_framebufferObject = nullptr;

public:
  FramebufferObjectRenderer(const ThreeDItem *item)
     : _item(item), _camera(75, item->width() / item->height(), 0.1, 1000 ),
       _renderer(item->width(), item->height())
  {
    _renderer.setClearColorHex(0xEEEEEE);
    _renderer.setSize(item->width(), item->height());
    AxisHelper axes(20);
    scene.add(axes);
    Plane plane(60, 20, 1, 1);
    MeshBasicMaterial planeMaterial(three::Color(0xcccccc));
    Mesh plane(planeGeometry, planeMaterial);
    plane.rotation().x = -0.5 * M_PI;
    plane.position.x = 15;
    plane.position.y = 0;
    plane.position.z = 0;

    scene.add(plane);
  }

  ~FramebufferObjectRenderer() {
  }

/*
  double viewportWidth() override {
    return m_item->width();
  }

  double viewportHeight() override {
    return m_item->height();
  }

  QMatrix4x4 &viewModelMatrix() override {
    return m_model;
  }
*/

  void synchronize(QQuickFramebufferObject *_item) override
  {
    ThreeDItem *item = static_cast<ThreeDItem *>(_item);

    Box cubeGeometry(4, 4, 4);
    MeshBasicMaterial cubeMaterial(Color(0xff0000), true);
    Mesh cube(cubeGeometry, cubeMaterial);
    cube.position.x = -4;
    cube.position.y = 3;
    cube.position.z = 0;
    scene.add(cube);

    Sphere sphereGeometry(4, 20, 20);
    MeshBasicMaterial sphereMaterial(Color(0x7777ff), true);
    Mesh sphere(sphereGeometry, sphereMaterial);
    sphere.position.x = 20;
    sphere.position.y = 4;
    sphere.position.z = 2;
    scene.add(sphere);

    camera.position.x = -30;
    camera.position.y = 40;
    camera.position.z = 30;
    camera.lookAt(scene.position);
  }

  void render() override
  {
    _renderer.render(_scene, _camera);
    _item->window()->resetOpenGLState();
  }

  QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
  {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(12);
    _framebufferObject = new QOpenGLFramebufferObject(size, format);

    return _framebufferObject;
  }
};

ThreeDItem::ThreeDItem(QQuickItem *parent)
   : QQuickFramebufferObject(parent), m_model(nullptr), m_interactor(new ThreeDInteractor())
{
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemAcceptsInputMethod);

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