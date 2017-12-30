//
// Created by cse on 5/12/17.
//

#ifndef VTKQUICK_VTK_3DSITEM_H
#define VTKQUICK_VTK_3DSITEM_H

#include <QQuickFramebufferObject>
#include <QColor>
#include <memory>
#include <scene/Scene.h>
#include <quick/interact/OrbitControls.h>

class QOpenGLShaderProgram;

namespace three {
namespace quick {

class ThreeDTestItem : public QQuickFramebufferObject
{
Q_OBJECT
  Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)

  QColor m_background;
  OrbitControls::Ptr _controls;

public:
  explicit ThreeDTestItem(QQuickItem *parent = 0);

  ~ThreeDTestItem() override;

  Renderer *createRenderer() const override;

  QColor background() { return m_background; }

  void setBackground(QColor background);

  QMetaObject::Connection _geometryUpdate;

  void setControls(OrbitControls::Ptr controls) {
    _controls = controls;
  }

protected:
  friend class FramebufferObjectRenderer;

  friend class RenderDelegate;

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
  void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

  void releaseResources() override;

signals:

  void backgroundChanged();

  void sceneGeometryChanged();
};

}
}

#endif //VTKQUICK_VTK_3DSITEM_H
