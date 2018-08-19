//
// Created by byter on 8/19/18.
//

#include "DragController.h"
#include <threepp/quick/cameras/Camera.h>

namespace three {
namespace quick {

void DragController::objectDragStarted(Object3D *object)
{
  for (const QVariant &var : _objects) {
    ThreeQObject *o = var.value<ThreeQObject *>();
    if (o && o->object().get() == object) {
      emit dragStarted(o);
      return;
    }
  }
}

void DragController::objectDropped(Object3D *object)
{
  for (const QVariant &var : _objects) {
    ThreeQObject *o = var.value<ThreeQObject *>();
    if (o && o->object().get() == object) {
      emit dropped(o);
      return;
    }
  }
}

void DragController::setItem(ThreeDItem *item)
{
  Interactor::setItem(item);
  if (_camera) _drag.setCamera(_camera->camera());
}

bool DragController::handleMousePressed(QMouseEvent *event)
{
  _startingPos = event->pos();
  return false;
}

bool DragController::handleMouseMoved(QMouseEvent *event)
{
  if (!_dragging) {
    QPoint point = event->pos() - _startingPos;
    if (point.manhattanLength() > 6) {
      _item->setCursor(_dragCursor);

      qreal x = (event->x() / _item->width()) * 2 - 1;
      qreal y = -(event->y() / _item->height()) * 2 + 1;

      _drag.mouseDown(x, y);
      _dragging = true;

      event->setAccepted(true);
    }
  }
  else {
    qreal x = (event->x() / _item->width()) * 2 - 1;
    qreal y = -(event->y() / _item->height()) * 2 + 1;

    _drag.mouseMove(x, y);
    event->setAccepted(true);
  }
  return true;
}

bool DragController::handleMouseReleased(QMouseEvent *event)
{
  if (_dragging) {
    _dragging = false;
    _item->unsetCursor();
    event->setAccepted(true);

    qreal x = (event->x() / _item->width()) * 2 - 1;
    qreal y = -(event->y() / _item->height()) * 2 + 1;

    _drag.mouseRelease(x, y);
    return true;
  }
  return false;
}

void DragController::setObjects(const QVariantList &objects)
{
  if (_objects != objects) {
    _objects = objects;

    std::vector<Object3D::Ptr> objs;
    for (const QVariant &var : objects) {
      ThreeQObject *o = var.value<ThreeQObject *>();
      if (o) objs.push_back(o->object());
    }
    _drag.setObjects(objs);

    emit objectsChanged();
  }
}

}
}