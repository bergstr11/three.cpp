//
// Created by byter on 1/29/18.
//
#include "RayCaster.h"

namespace three {
namespace quick {

using namespace std;

void RayCaster::setFromCamera(const QVector2D &mouse, Camera *camera)
{

}

QList<ThreeQObject *> RayCaster::intersectObjects(const QList<ThreeQObject *> &objects)
{
  vector<Object3D::Ptr> objs;
  QList<ThreeQObject *> result;

  for(auto o : objects) objs.push_back(o->object());
  vector<Intersection> intersects = raycaster.intersectObjects(objs);

  return result;
}

void RayCaster::addTo(ObjectRootContainer *container)
{

}

}
}