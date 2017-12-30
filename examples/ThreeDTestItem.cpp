//
// Created by cse on 5/12/17.
//

#include <functional>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QThread>
#include <QScreen>

#include "ThreeDTestItem.h"

#include <renderers/OpenGLRenderer.h>
#include <objects/Mesh.h>
#include <scene/Scene.h>
#include <camera/PerspectiveCamera.h>
#include <material/MeshLambertMaterial.h>
#include <material/ShaderMaterial.h>
#include <textures/ImageTexture.h>
#include <geometry/Box.h>
#include <geometry/Plane.h>
#include <geometry/Sphere.h>
#include <helper/Axes.h>
#include <helper/SpotLight.h>
#include <light/AmbientLight.h>
#include <renderers/gl/shader/ShaderLib.h>

namespace three {
namespace quick {

using namespace std;
using namespace three;
using namespace three::geometry;
using namespace three::gl;

namespace threeDItem {

class FramebufferObjectRenderer : public QObject, public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
Q_OBJECT

  QColor m_background;

  Scene::Ptr _scene, _sceneCube;
  PerspectiveCamera::Ptr _camera, _cameraCube;
  OpenGLRenderer::Ptr _renderer;
  three::Renderer::Target::Ptr _target;
  helper::SpotLight::Ptr _lightHelper;

  QOpenGLFramebufferObject *_fbo;
  const ThreeDTestItem *const _item;
  MeshT<buffer::Sphere, MeshLambertMaterial>::Ptr _sphereMesh;

public:

  explicit FramebufferObjectRenderer(const ThreeDTestItem *item)
     : _item(item),
       _scene(Scene::make("scene")),
       _sceneCube(Scene::make("sceneCube")),
       _camera(PerspectiveCamera::make(70, item->width() / item->height(), 1, 100000)),
       _cameraCube(PerspectiveCamera::make(70, item->width() / item->height(), 1, 100000)),
       _renderer(OpenGLRenderer::make(
          QOpenGLContext::currentContext(),
          item->width(), item->height(),
          item->window()->screen()->devicePixelRatio()))
  {
    _renderer->setSize(item->width(), item->height());
    _renderer->setShadowMapType(ShadowMapType::PCFSoft);

    TextureOptions cubeOptions = ImageCubeTexture::options();
    cubeOptions.format = TextureFormat::RGBA;
    cubeOptions.mapping = TextureMapping::CubeReflection;
    cubeOptions.flipY = false;
    cubeOptions.type = TextureType::UnsignedByte;
    CubeTexture::Ptr textureCube = ImageCubeTexture::make(cubeOptions,
                                                         {
                                                            QImage(":/cube_bridge2_posx.jpg").convertToFormat(QImage::Format_RGBA8888),
                                                            QImage(":/cube_bridge2_negx.jpg").convertToFormat(QImage::Format_RGBA8888),
                                                            QImage(":/cube_bridge2_posy.jpg").convertToFormat(QImage::Format_RGBA8888),
                                                            QImage(":/cube_bridge2_negy.jpg").convertToFormat(QImage::Format_RGBA8888),
                                                            QImage(":/cube_bridge2_posz.jpg").convertToFormat(QImage::Format_RGBA8888),
                                                            QImage(":/cube_bridge2_negz.jpg").convertToFormat(QImage::Format_RGBA8888)
                                                         });

    _camera->position().set(0, 0, 1000);
    _camera->lookAt(_scene->position());

    _scene->add(AmbientLight::make(0xffffff));

    TextureOptions equirecOptions = ImageTexture::options();
    equirecOptions.format = TextureFormat::RGBA;
    equirecOptions.mapping = TextureMapping::EquirectangularReflection;
    equirecOptions.magFilter = TextureFilter::Linear;
    equirecOptions.minFilter = TextureFilter::LinearMipMapLinear;
    Texture::Ptr textureEquirec = ImageTexture::make(equirecOptions, QImage(":/2294472375_24a3b8ef46_o.jpg"));

    TextureOptions sphereOptions = ImageTexture::options();
    sphereOptions.format = TextureFormat::RGBA;
    sphereOptions.mapping = TextureMapping::SphericalReflection;
    ImageTexture::Ptr textureSphere = ImageTexture::make(sphereOptions, QImage(":/metal.jpg"));

    ShaderInfo equirectShader = shaderlib::get(ShaderID::equirect);
    ShaderMaterial::Ptr equirectMaterial = ShaderMaterial::make(equirectShader.uniforms,
                                                                equirectShader.vertexShader,
                                                                equirectShader.fragmentShader,
                                                                Side::Back,
                                                                true,
                                                                false);
    equirectMaterial->uniforms.set(UniformName::tEquirect, textureEquirec);

    ShaderInfo cubeShader = shaderlib::get(ShaderID::cube);
    ShaderMaterial::Ptr cubeMaterial = ShaderMaterial::make(cubeShader.uniforms,
                                                            cubeShader.vertexShader,
                                                            cubeShader.fragmentShader,
                                                            Side::Back,
                                                            false,
                                                            false);
    cubeMaterial->uniforms.set(UniformName::tCube, textureCube);

    // Skybox
    auto cubeMesh = MeshT<buffer::Box, ShaderMaterial>::make("box", buffer::Box::make(100, 100, 100), cubeMaterial);
    _sceneCube->add(cubeMesh);

    //Sphere
    _sphereMesh = MeshT<buffer::Sphere, MeshLambertMaterial>::make("sphere",
                                                                       buffer::Sphere::make(400.0f, 48, 24),
                                                                       MeshLambertMaterial::make());
    _sphereMesh->material<0>()->envMap = textureCube;
    _scene->add(_sphereMesh);

    _renderer->autoClear = false;
    _renderer->setFaceCulling(CullFace::None, FrontFaceDirection::Undefined);
  }

  ~FramebufferObjectRenderer() override = default;

  void synchronize(QQuickFramebufferObject *_item) override
  {
    auto *item = static_cast<ThreeDTestItem *>(_item);
    if (!item->_geometryUpdate) {
      item->_geometryUpdate = QObject::connect(item, &ThreeDTestItem::sceneGeometryChanged,
                                               this, &FramebufferObjectRenderer::sceneGeometryChanged,
                                               Qt::QueuedConnection);

      OrbitControls::Ptr controls = OrbitControls::make(item, _camera);
      controls->minDistance = 500;
      controls->maxDistance = 2500;
      item->setControls(controls);
    }
  }

  void render() override
  {
    _camera->lookAt(_scene->position());
    _cameraCube->rotation() = _camera->rotation();

    _renderer->render(_sceneCube, _cameraCube, _target, true);
    //_fbo->bind();
    //_sphereMesh->material<0>()->needsUpdate = true;
    //_target->setReuse(true);
    _renderer->render(_scene, _camera, _target);

    _item->window()->resetOpenGLState();
  }

  QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
  {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setMipmap(false);
    format.setInternalTextureFormat(GL_RGBA);

    _fbo = new QOpenGLFramebufferObject(size, format);

    _target = OpenGLRenderer::makeExternalTarget(
       _fbo->handle(), _fbo->texture(), _item->width(), _item->height());

    return _fbo;
  }

public slots:

  void sceneGeometryChanged()
  {
    _camera->setAspect(_item->width() / _item->height());
    _camera->updateProjectionMatrix();

    _cameraCube->setAspect(_item->width() / _item->height());
    _cameraCube->updateProjectionMatrix();

    _renderer->setSize(_item->width(), _item->height());
  };
};

}

ThreeDTestItem::ThreeDTestItem(QQuickItem *parent) : QQuickFramebufferObject(parent)
{
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemAcceptsInputMethod);

  setMirrorVertically(true);
}

ThreeDTestItem::~ThreeDTestItem()
{
}

QQuickFramebufferObject::Renderer *ThreeDTestItem::createRenderer() const
{
  return new threeDItem::FramebufferObjectRenderer(this);
}

void ThreeDTestItem::setBackground(QColor background) {
  if (m_background != background) {
    m_background = background;
    emit backgroundChanged();
  }
}

void ThreeDTestItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
  QQuickFramebufferObject::geometryChanged(newGeometry, oldGeometry);
  sceneGeometryChanged();
}

void ThreeDTestItem::mouseMoveEvent(QMouseEvent *event) {
  if(_controls && _controls->handleMouseMoved(event)) update();
}

void ThreeDTestItem::mousePressEvent(QMouseEvent *event) {
  if(_controls && _controls->handleMousePressed(event)) update();
}

void ThreeDTestItem::mouseReleaseEvent(QMouseEvent *event) {
  if(_controls && _controls->handleMouseReleased(event)) update();
}

void ThreeDTestItem::mouseDoubleClickEvent(QMouseEvent *event) {
  update();
}

void ThreeDTestItem::wheelEvent(QWheelEvent *event) {
  if(_controls && _controls->handleMouseWheel(event)) update();
}


void ThreeDTestItem::keyPressEvent(QKeyEvent *event) {
  update();
}

void ThreeDTestItem::keyReleaseEvent(QKeyEvent *event) {
  update();
}

void ThreeDTestItem::focusInEvent(QFocusEvent *event) {
  forceActiveFocus();
}

void ThreeDTestItem::focusOutEvent(QFocusEvent *event) {
  update();
}

void ThreeDTestItem::releaseResources() {
  QQuickFramebufferObject::releaseResources();
}

}
}

#include "ThreeDTestItem.moc"
