//
// Created by byter on 1/29/18.
//
#include "RayCaster.h"

namespace three {
namespace quick {

using namespace std;

void RayCaster::set(const QVector2D &position)
{
  _camera->camera()->setup(raycaster.ray(), position.x(), position.y());
}

QVariantList RayCaster::intersectObjects(const QVariantList &objects)
{
  vector<Object3D::Ptr> objs;
  QVariantList result;

  for(const QVariant &var : objects) {
    ThreeQObject *o = var.value<ThreeQObject *>();
    objs.push_back(o->object());
  }
  vector<Intersection> intersects = raycaster.intersectObjects(objs);

  for(const QVariant &ovar : objects) {
    ThreeQObject *to = ovar.value<ThreeQObject *>();
    auto found = find_if(intersects.begin(), intersects.end(),
                         [&to](Intersection &i) {return i.object == to->object().get();});

    if(found != intersects.end()) {
      QVariant var;
      var.setValue(new Intersect(to, *found));
      result.push_back(var);
    }
  }

  return result;
}

}
}