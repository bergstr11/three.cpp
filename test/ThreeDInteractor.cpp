//
// Created by cse on 6/4/17.
//
#include <math.h>

#include "ThreeDInteractor.h"

namespace lo {
namespace ui {
namespace quick {

using namespace std;

/*
ThreeDInteractor::ThreeDInteractor(shared_ptr<AbstractMover> leftMover,
                                   shared_ptr<AbstractMover> midMover,
                                   shared_ptr<AbstractMover> rightMover)
   : m_leftButtonMover(leftMover), m_midButtonMover(midMover), m_rightButtonMover(rightMover)
{

}

void ThreeDInteractor::setViewport(Viewport *viewport)
{
  if(m_leftButtonMover) m_leftButtonMover->setViewport(viewport);
  if(m_midButtonMover) m_midButtonMover->setViewport(viewport);
  if(m_rightButtonMover) m_rightButtonMover->setViewport(viewport);
}
*/

bool ThreeDInteractor::processPendingEvent()
{
  if(!eventSet) return false;

  QEvent *e = eventSet;
  eventSet = nullptr;

  const QEvent::Type t = e->type();

  switch(t) {
    case QEvent::MouseMove: {
      return processMouseMove(static_cast<QMouseEvent*>(e));
    }
    case QEvent::MouseButtonPress: {
      return processMousePress(static_cast<QMouseEvent*>(e));
    }
    case QEvent::MouseButtonRelease: {
      return processMouseRelease(static_cast<QMouseEvent*>(e));
    }
    case QEvent::MouseButtonDblClick: {
      break;
    }
  }

  return false;
}

bool ThreeDInteractor::processMouseRelease(QMouseEvent *e)
{
  Q_UNUSED(e);
  bool subject= false;

  if (m_MovingCamera)
  {
    subject= true;
    m_MovingCamera = false;
  }
  else if ((e->button() == Qt::LeftButton))
  {
    //select(e->x(), e->y(), GLC_SelectionEvent::ModeReplace | m_pViewHandler->selectionMode());
    subject= true;
  }

  return subject;
}

bool ThreeDInteractor::processMousePress(QMouseEvent *e)
{
  bool subject= false;
  if (!m_MovingCamera)
  {
    m_StartingPos= e->pos();
    m_EventButton= e->button();
  }

  return subject;
}

bool ThreeDInteractor::processMouseMove(QMouseEvent *e)
{
  bool processed = false;

/*
  if (!m_MovingCamera)
  {
    QPoint point= e->pos() - m_StartingPos;
    if (point.manhattanLength() > 5)
    {
      switch (m_EventButton)
      {
        case (Qt::LeftButton):
          m_activeMover = m_leftButtonMover;
          break;
        case (Qt::MidButton):
          m_activeMover = m_midButtonMover;
          break;
        case (Qt::RightButton):
          m_activeMover = m_rightButtonMover;
          break;
        default:
          break;
      }
      m_activeMover->start(e->x(), e->y());
      m_MovingCamera = true;
      processed = true;
    }
  }
  else
  {
    m_activeMover->move(e->x(), e->y());
    processed = true;
  }
*/

  return processed;
}

bool ThreeDInteractor::processMouseDblClick(QMouseEvent *event)
{
  if ((event->button() == Qt::LeftButton))
  {
    //select(event->x(), event->y(), GLC_SelectionEvent::ModeReplace | m_pViewHandler->selectionMode());

    event->accept();
    return true;
  }
  return false;
}

bool ThreeDInteractor::processWheel(QWheelEvent *event)
{
  QPoint numDegrees = event->angleDelta() / 8;
  if (!numDegrees.isNull()) {
    double factor = numDegrees.y() > 0 ? 1.1 : 0.9;
    //m_pViewHandler->viewportHandle()->cameraHandle()->zoom(factor);
    //m_pViewHandler->updateGL();
  }
  event->accept();

  return true;
}

double dist(const QPointF &p1, const QPointF &p2) {
  double x = p1.x() - p2.x();
  double y = p1.y() - p2.y();
  return sqrt(x*x + y*y);
}

bool ThreeDInteractor::processTouch(QTouchEvent *pTouchEvent)
{
  if(pTouchEvent->type() == QEvent::TouchEnd || pTouchEvent->type() == QEvent::TouchCancel) {
    m_touchDist = 0;
  }
  else if(pTouchEvent->touchPoints().count() == 2) {
    if(pTouchEvent->type() == QEvent::TouchBegin || m_touchDist == 0) {
      QPointF p1 = pTouchEvent->touchPoints()[0].normalizedPos();
      QPointF p2 = pTouchEvent->touchPoints()[1].normalizedPos();
      m_touchDist = dist(p1, p2);
      pTouchEvent->accept();
      return true;
    }
    else if(pTouchEvent->type() == QEvent::TouchUpdate) {
      QPointF p1 = pTouchEvent->touchPoints()[0].normalizedPos();
      QPointF p2 = pTouchEvent->touchPoints()[1].normalizedPos();
      double d = dist(p1, p2);
      double diff = m_touchDist - d;
      //m_pViewHandler->viewportHandle()->cameraHandle()->zoom(1+diff);
      //m_pViewHandler->updateGL();
      m_touchDist = d;
      pTouchEvent->accept();
      return true;
    }
  }
  pTouchEvent->ignore();
  return false;
}

}
}
}