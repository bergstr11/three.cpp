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

#include <OpenGLRenderer>
#include <camera/PerspectiveCamera>
#include <Scene>
#include <Mesh>
#include <material/MeshBasicMaterial>
#include <geometry/Box>

#include "modelloader.h"

namespace lo {
namespace ui {
namespace quick {

using namespace std;
using namespace lo::threed;

class FramebufferObjectRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions_3_0
{
  Light m_Light;
  Material m_defaultMaterial;

  QColor m_background;

  three::Scene m_scene;
  three::PerspectiveCamera m_camera;
  three::OpenGLRenderer m_renderer;

  const ThreeDItem *const m_item;
  QOpenGLFramebufferObject *m_framebufferObject = nullptr;

public:
  FramebufferObjectRenderer(const ThreeDItem *item)
     : m_item(item), m_camera(75, item->width() / item->height(), 0.1, 1000 ),
       m_renderer(item->width(), item->height())
  {
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
    const Model::Ptr model = item->pendingModel();
    if (model) {
      three::geometry::Box geometry {1, 1, 1};
      three::MeshBasicMaterial material(0x00ff00);
      three::Mesh cube( geometry, material );
      m_scene.add( cube );

      m_camera.position.z = 5;
    }

    m_background = item->m_background;
  }

  void render() override
  {
    m_renderer.render(m_scene, m_camera);
    m_item->window()->resetOpenGLState();
  }

  QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
  {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(12);
    m_framebufferObject = new QOpenGLFramebufferObject(size, format);

    return m_framebufferObject;
  }
};

ThreeDItem::ThreeDItem(QQuickItem *parent)
   : QQuickFramebufferObject(parent), m_model(nullptr), m_interactor(new ThreeDInteractor())
{
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemAcceptsInputMethod);

  m_loader = new ModelLoader();
  connect(m_loader, &ModelLoader::modelLoaded, this, &ThreeDItem::modelLoaded, Qt::QueuedConnection);
}

ThreeDItem::~ThreeDItem()
{
  delete m_interactor;
  delete m_loader;
}

QQuickFramebufferObject::Renderer *ThreeDItem::createRenderer() const
{
  return new FramebufferObjectRenderer(this);
}

void ThreeDItem::setData(QString data) {
  if (data != m_data) {
    m_data = data;

    m_loader->asynchLoad(QFileInfo(m_data));
  }
}

void ThreeDItem::modelLoaded()
{
  m_model = m_loader->getLoaded();
  m_pendingModel = true;
  update();
  emit dataChanged();
}


void ThreeDItem::setBackground(QColor background) {
  if (m_background != background) {
    m_background = background;
    emit backgroundChanged();
  }
}

const shared_ptr<Model> ThreeDItem::pendingModel()
{
  if (m_pendingModel) {
    m_pendingModel = false;
    return m_model;
  }
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