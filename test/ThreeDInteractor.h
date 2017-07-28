//
// Created by cse on 6/4/17.
//

#ifndef QGL_TEST_THREEDMANIPULATOR_H
#define QGL_TEST_THREEDMANIPULATOR_H

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QCloseEvent>
#include <QPoint>

#include <memory>

namespace lo {
namespace ui {
namespace quick {

class AbstractMover;

class ThreeDInteractor
{
public:

  bool processPendingEvent();

  void SetEvent(QMouseEvent* e) {
    QEvent *es = eventSet;
    if(es && e->type() != es->type()) return;
    mouseEvent = *e; eventSet = &mouseEvent;
  }
  void SetEvent(QWheelEvent* e) {
    QEvent *es = eventSet;
    if(es && e->type() != es->type()) return;
    wheelEvent = *e; eventSet = &wheelEvent;
  }
  void SetEvent(QKeyEvent* e) {
    QEvent *es = eventSet;
    if(es && e->type() != es->type()) return;
    keyEvent = *e; eventSet = &keyEvent;
  }
  void SetEvent(QFocusEvent* e) {
    QEvent *es = eventSet;
    if(es && e->type() != es->type()) return;
    focusEvent = *e; eventSet = &focusEvent;
  }
  void SetEvent(QCloseEvent* e) {
    QEvent *es = eventSet;
    if(es && e->type() != es->type()) return;
    closeEvent = *e; eventSet = &closeEvent;
  }

  //void setViewport(Viewport *viewport);

protected:
  QMouseEvent mouseEvent {QEvent::None, QPointF(0, 0), Qt::NoButton, Qt::NoButton, 0};
  QWheelEvent wheelEvent {QPoint(0, 0), 0, Qt::NoButton, 0};
  QKeyEvent keyEvent {QEvent::None, Qt::NoButton, 0};
  QFocusEvent focusEvent {QEvent::None};
  QCloseEvent closeEvent;

  QEvent *eventSet = nullptr;

//  std::shared_ptr<AbstractMover> m_activeMover;
//  std::shared_ptr<AbstractMover> m_leftButtonMover;
//  std::shared_ptr<AbstractMover> m_midButtonMover;
//  std::shared_ptr<AbstractMover> m_rightButtonMover;

  bool m_MovingCamera;
  double m_touchDist;
  QPoint m_StartingPos;
  Qt::MouseButton m_EventButton;

  bool processMousePress(QMouseEvent *e);
  bool processMouseMove(QMouseEvent *e);
  bool processMouseRelease(QMouseEvent *e);
  bool processMouseDblClick(QMouseEvent *e);
  bool processWheel(QWheelEvent *e);
  bool processTouch(QTouchEvent *pTouchEvent);
};


}
}
}

#endif //QGL_TEST_THREEDMANIPULATOR_H
