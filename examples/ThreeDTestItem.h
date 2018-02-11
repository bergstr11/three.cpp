//
// Created by cse on 5/12/17.
//

#ifndef THREEPP_TESTITEM_H
#define THREEPP_TESTITEM_H

#include <QQuickFramebufferObject>
#include <QColor>
#include <memory>
#include <threepp/scene/Scene.h>
#include <threepp/quick/interact/OrbitControls.h>
#include <threepp/renderers/OpenGLRenderer.h>

class QOpenGLShaderProgram;

namespace three {
namespace quick {

namespace threeDItem {
 class FramebufferObjectRenderer;
}

class ThreeDTestItem : public QQuickFramebufferObject
{
Q_OBJECT
  Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)

  friend class threeDItem::FramebufferObjectRenderer;

  QColor m_background;
  OrbitControls::Ptr _controls;
  three::OpenGLRenderer::Ptr _renderer;

public:
  explicit ThreeDTestItem(QQuickItem *parent = 0);

  ~ThreeDTestItem() override;

  Renderer *createRenderer() const override;

  QColor background() { return m_background; }

  void setBackground(QColor background);

  void setControls(OrbitControls::Ptr controls) {
    _controls = controls;
  }

protected:
  friend class FramebufferObjectRenderer;

  friend class RenderDelegate;

  void componentComplete() override;

  void mouseMoveEvent(QMouseEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;

  void mouseReleaseEvent(QMouseEvent *event) override;

  void mouseDoubleClickEvent(QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void focusInEvent(QFocusEvent *event) override;

  void focusOutEvent(QFocusEvent *event) override;

protected:
  void releaseResources() override;

signals:

  void backgroundChanged();

  void sceneGeometryChanged();
};

}
}

#endif //THREEPP_TESTITEM_H
