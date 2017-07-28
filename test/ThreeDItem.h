//
// Created by cse on 5/12/17.
//

#ifndef VTKQUICK_VTK_3DSITEM_H
#define VTKQUICK_VTK_3DSITEM_H

#include <QQuickFramebufferObject>
#include <QColor>
#include <memory>

class QOpenGLShaderProgram;

namespace lo {
namespace threed {
class Model;
class ModelLoader;
}
}

namespace lo {
namespace ui {
namespace quick {

class ThreeDInteractor;

class ThreeDItem : public QQuickFramebufferObject
{
Q_OBJECT
  Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)
  Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)

  std::shared_ptr<lo::threed::Model> m_model;
  ThreeDInteractor * const m_interactor;
  QString m_data;
  QColor m_background;
  bool m_pendingModel = false;
  lo::threed::ModelLoader *m_loader;

public:
  ThreeDItem(QQuickItem *parent = 0);

  ~ThreeDItem();

  Renderer *createRenderer() const;

  QString data() { return m_data; }

  void setData(QString data);

  QColor background() { return m_background; }

  void setBackground(QColor background);

  const std::shared_ptr<lo::threed::Model> pendingModel();

private slots:

  void modelLoaded();

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

  void dataChanged();

  void backgroundChanged();
};

}
}
}

#endif //VTKQUICK_VTK_3DSITEM_H
