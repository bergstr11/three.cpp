//
// Created by byter on 1/29/18.
//
#include "RayCaster.h"

namespace three {
namespace quick {

using namespace std;

void RayCaster::set(const QVector2D &position)
{
  if(_camera && _camera->camera()) {
    _raycaster.set(_camera->camera()->ray(position.x(), position.y()));
  }
}

QVariantList RayCaster::intersectObjects(const QVariantList &objects)
{
  vector<Object3D::Ptr> objs;
  QVariantList result;

  for(const QVariant &var : objects) {
    ThreeQObject *o = var.value<ThreeQObject *>();
    objs.push_back(o->object());
  }
  IntersectList intersects;
  _raycaster.intersectObjects(objs, intersects);
  for(const Intersection &isect : intersects) {

    auto found = find_if(objects.begin(), objects.end(),
                         [&isect](QVariant var) {
                           ThreeQObject *o = var.value<ThreeQObject *>();
                           return isect.object == o->object().get();});

    if(found != objects.end()) {
      QVariant var;
      var.setValue(new Intersect(found->value<ThreeQObject *>(), isect));
      result.push_back(var);
      break;
    }
  }

  return result;
}

}
}