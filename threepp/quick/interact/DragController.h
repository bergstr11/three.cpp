//
// Created by byter on 8/18/18.
//

#ifndef THREE_PP_DRAGCONTROLLER_H
#define THREE_PP_DRAGCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QCursor>
#include <threepp/controls/Drag.h>
#include <threepp/quick/ThreeDItem.h>
#include <threepp/quick/ThreeQObjectRoot.h>

namespace three {
namespace quick {

class DragController : public ThreeQObjectRoot, public Interactor
{
  Q_OBJECT
  Q_PROPERTY(three::quick::Camera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
  Q_PROPERTY(QVariantList objects READ objects WRITE setObjects NOTIFY objectsChanged)
  Q_PROPERTY(Qt::CursorShape dragCursor READ dragCursor WRITE setDragCursor NOTIFY dragCursorChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(QVariantList surface READ surface WRITE setSurface NOTIFY surfaceChanged)

  Camera *_camera = nullptr;
  QVariantList _objects;
  QVariantList _surface;

  QCursor _dragCursor {Qt::DragMoveCursor};

  control::Drag _drag;

  bool _dragging = false;
  QPoint _startingPos;

protected:
  bool handleMousePressed(QMouseEvent *event) override;
  bool handleMouseMoved(QMouseEvent *event) override;
  bool handleMouseReleased(QMouseEvent *event) override;

  QVariantList objects() {return _objects;}

  void setObjects(const QVariantList &objects);

public:
  explicit DragController(QObject *parent = nullptr) : ThreeQObjectRoot(parent)
  {
    _drag.onDragStarted.connect(*this, &DragController::objectDragStarted);
    _drag.onDropped.connect(*this, &DragController::objectDropped);
  }

  bool enabled() {return Interactor::_enabled;}

  void objectDragStarted(Object3D *object);
  void objectDropped(Object3D *object);

  void setEnabled(bool enabled)
  {
    if(Interactor::_enabled != enabled) {
      Interactor::_enabled = enabled;
      emit enabledChanged();
    }
  }

  void setItem(ThreeDItem * item) override;

  Camera *camera() const {return _camera;}

  void setCamera(Camera *camera)
  {
    if(camera && _camera != camera) {
      _camera = camera;
      emit cameraChanged();
    }
  }

  Qt::CursorShape dragCursor() const { return _dragCursor.shape(); }

  void setDragCursor(Qt::CursorShape cursor)
  {
    if (_dragCursor.shape() != cursor) {
      _dragCursor = cursor;
      emit dragCursorChanged();
    }
  }

  const QVariantList &surface() const {return _surface;}
  void setSurface(const QVariantList &surface);

signals:
  void cameraChanged();
  void dragCursorChanged();
  void enabledChanged();
  void objectsChanged();
  void surfaceChanged();
  void dragStarted(ThreeQObject *);
  void dropped(ThreeQObject *);
};

}
}

#endif //THREE_PP_DRAGCONTROLLER_H
