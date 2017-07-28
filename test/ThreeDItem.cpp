//
// Created by cse on 5/12/17.
//

#include <functional>

#include <QOpenGLFunctions_3_0>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QThread>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QResource>

#include "ThreeDItem.h"
#include "ThreeDInteractor.h"

#include "modelloader.h"

namespace lo {
namespace ui {
namespace quick {

using namespace std;
using namespace lo::threed;

class FramebufferObjectRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions_3_0
{
  QOpenGLShaderProgram m_program;

  QOpenGLBuffer m_vertexBuffer;
  QOpenGLBuffer m_normalBuffer;
  QOpenGLBuffer m_textureUVBuffer;
  QOpenGLBuffer m_indexBuffer;

  shared_ptr<Node> m_rootNode;

  QOpenGLVertexArrayObject m_vao;

  QMatrix4x4 m_projection, m_view, m_model;

  Light m_Light;
  Material m_defaultMaterial;

  QColor m_background;

  const ThreeDItem *const m_item;
  QOpenGLFramebufferObject *m_framebufferObject = nullptr;


  bool createProgram() {
    QResource vert(":/vertex.glsl");
    QResource frag(":/fragment.glsl");

    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, (char *) vert.data())) {
      qWarning() << m_program.log();
      return false;
    }
    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, (char *) frag.data())) {
      qWarning() << m_program.log();
      return false;
    }

    // OpenGL 2.1/OpenGL ES -- Vertex shader attributes need to be mapped to location before shader is linked
    m_program.bindAttributeLocation("vertexPosition", 0);
    m_program.bindAttributeLocation("vertexNormal", 1);

    if (!m_program.link()) {
      qWarning() << m_program.log();
      return false;
    }
    return true;
  }

  bool createBuffers(const Model::Ptr model)
  {
    const vector<float> *vertices;
    const vector<float> *normals;
    const vector<vector<float> > *textureUV;
    const vector<unsigned int> *indices;

    model->getBufferData(&vertices, &normals, &indices);
    model->getTextureData(&textureUV, 0, 0);

    // Create a vertex array object
    if (!m_vao.isCreated()) m_vao.create();
    m_vao.bind();

    //create data buffers
    if (!m_vertexBuffer.isCreated()) {
      m_vertexBuffer.create();
      m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(&(*vertices)[0], vertices->size() * sizeof(float));

    if (!m_normalBuffer.isCreated()) {
      m_normalBuffer.create();
      m_normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_normalBuffer.bind();
    m_normalBuffer.allocate(&(*normals)[0], normals->size() * sizeof(float));

    if (textureUV != 0 && textureUV->size() != 0) {
      if (!m_textureUVBuffer.isCreated()) {
        m_textureUVBuffer.create();
        m_textureUVBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
      }
      m_textureUVBuffer.bind();

      int texSize = 0;
      for (int ii = 0; ii < textureUV->size(); ++ii)
        texSize += textureUV->at(ii).size();

      m_textureUVBuffer.allocate(&(*textureUV)[0][0], texSize * sizeof(float));
    }

    // Create a buffer and copy the index data to it
    if (!m_indexBuffer.isCreated()) {
      m_indexBuffer.create();
      m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_indexBuffer.bind();
    m_indexBuffer.allocate(&(*indices)[0], indices->size() * sizeof(unsigned int));

    m_rootNode = model->getNodeData();
  }

  void setupLightingAndMatrices()
  {
    m_view.setToIdentity();
    m_view.lookAt(
       QVector3D(0.0f, 0.0f, 1.2f),    // Camera Position
       QVector3D(0.0f, 0.0f, 0.0f),    // Point camera looks towards
       QVector3D(0.0f, 1.0f, 0.0f));   // Up vector

    // Set the model matrix
    // Translate and rotate it a bit to get a better view of the model
    m_model.setToIdentity();
    m_model.translate(-0.1f, 0.0f, .2f);
    m_model.rotate(55.0f, 0.0f, 1.0f, 0.0f);

    float aspect = (float) 2;//(viewportWidth() / viewportHeight());
    m_projection.setToIdentity();
    m_projection.perspective(
       60.0f,          // field of vision
       aspect,         // aspect ratio
       0.3f,           // near clipping plane
       1000.0f);       // far clipping plane

    m_Light.position = QVector4D(-1.0f, 1.0f, 1.0f, 1.0f);
    m_Light.intensity = QVector3D(1.0f, 1.0f, 1.0f);

    m_defaultMaterial.ambient = QVector3D(0.05f, 0.2f, 0.05f);
    m_defaultMaterial.diffuse = QVector3D(0.3f, 0.5f, 0.3f);
    m_defaultMaterial.specular = QVector3D(0.6f, 0.6f, 0.6f);
    m_defaultMaterial.shininess = 50.0f;
    m_defaultMaterial.opacity = 1.0;
  }

  void createAttributes()
  {
    m_vao.bind();
    m_program.bind();

    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 3);
    m_program.enableAttributeArray(0);

    // Map normal data to the vertex shader's layout location '1'
    m_normalBuffer.bind();
    m_program.setAttributeBuffer(1, GL_FLOAT, 0, 3);
    m_program.enableAttributeArray(1);

    if (m_textureUVBuffer.isCreated()) {
      m_textureUVBuffer.bind();
      m_program.setAttributeBuffer(2, GL_FLOAT, 0, 2);
      m_program.enableAttributeArray(2);
    }
  }

  void setMaterialUniforms(const Material& material)
  {
    m_program.setUniformValue("mat.ambient",   material.ambient);
    m_program.setUniformValue("mat.diffuse",   material.diffuse);
    m_program.setUniformValue("mat.specular",  material.specular);
    m_program.setUniformValue("mat.shininess", material.shininess);
    m_program.setUniformValue("mat.alpha",     material.opacity);
  }

  void drawNode(const Node::Ptr node, QMatrix4x4 objectMatrix)
  {
    // Prepare matrices
    objectMatrix *= node->getTransformation();
    QMatrix4x4 modelMatrix = m_model * objectMatrix;
    QMatrix4x4 modelViewMatrix = m_view * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projection * modelViewMatrix;

    m_program.setUniformValue("MV", modelViewMatrix);// Transforming to eye space
    m_program.setUniformValue("N", normalMatrix);    // Transform normal to Eye space
    m_program.setUniformValue("MVP", mvp);           // Matrix for transforming to Clip space

    // Draw each mesh in this node
    for (int i = 0; i < node->meshes.size(); ++i) {
      const Mesh& mesh = *node->meshes[i];

      if (mesh.material->name == QString("DefaultMaterial"))
        setMaterialUniforms(m_defaultMaterial);
      else
        setMaterialUniforms(*mesh.material);

      glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (const GLvoid*)(mesh.indexOffset * sizeof(GLuint)));
    }

    // Recursively draw this nodes children nodes
    for (int inn = 0; inn < node->children.size(); ++inn)
      drawNode(node->children[inn], objectMatrix);
  }

public:
  FramebufferObjectRenderer(const ThreeDItem *item)
     : m_item(item), m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
       m_indexBuffer(QOpenGLBuffer::IndexBuffer)
  {
    initializeOpenGLFunctions();

    if (createProgram()) {
      setupLightingAndMatrices();

      glEnable(GL_DEPTH_TEST);
    }
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
      //item->m_interactor->setViewport(this);
      createBuffers(model);
      createAttributes();
    }
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, (float)(item->width()/item->height()), .3f, 1000);

    m_background = item->m_background;
  }

  void render() override
  {
    // Clear color and depth buffers
    int r, g, b, a;
    m_background.getRgb(&r, &g, &b, &a);
    glClearColor((float) r, (float) g, (float) b, (float) a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    //handle user requests
    m_item->m_interactor->processPendingEvent();

    if (m_rootNode) {
      // Bind shader program
      m_program.bind();

      // Set shader uniforms for light information
      m_program.setUniformValue("light.position", m_Light.position);
      m_program.setUniformValue("light.intensity", m_Light.intensity);

      // Bind VAO and draw everything
      m_vao.bind();
      drawNode(m_rootNode, QMatrix4x4());
      m_vao.release();
    }

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