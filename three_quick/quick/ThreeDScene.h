//
// Created by cse on 5/12/17.
//

#ifndef THREEPP_QUICK_SCENE_H
#define THREEPP_QUICK_SCENE_H

#include <QQuickFramebufferObject>
#include <QColor>
#include <memory>
#include <scene/Scene.h>
#include <camera/Camera.h>
#include "quick/objects/ThreeDObject.h"
#include "quick/cameras/Camera.h"

namespace three {
namespace quick {

class ThreeDScene : public QQuickFramebufferObject
{
Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
  Q_PROPERTY(Camera * camera READ camera WRITE setCamera NOTIFY cameraChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeDObject> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  QString _name;
  QColor _background;
  QList<ThreeDObject *> _objects;

  Camera *_quickCamera = nullptr;

  three::Camera::Ptr _camera;
  three::Scene::Ptr _scene;

  static void append_object(QQmlListProperty<ThreeDObject> *list, ThreeDObject *obj);
  static int count_objects(QQmlListProperty<ThreeDObject> *);
  static ThreeDObject *object_at(QQmlListProperty<ThreeDObject> *, int);
  static void clear_objects(QQmlListProperty<ThreeDObject> *);

public:
  static void init();

  explicit ThreeDScene(QQuickItem *parent = 0);

  ~ThreeDScene() override;

  QQmlListProperty<ThreeDObject> objects();

  Renderer *createRenderer() const override;

  QColor background() { return _background; }

  void setBackground(const QColor &background);

  QString name() {return _name;}

  void setName(const QString &name);

  Camera *camera() {return _quickCamera;}

  void setCamera(Camera *camera);

  three::Scene::Ptr scene() {return _scene;}

protected:
  void componentComplete() override;

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
  void nameChanged();
  void cameraChanged();
};

}
}

#endif //THREEPP_QUICK_SCENE_H
