//
// Created by byter on 1/29/18.
//

#include "GridLines.h"
#include <threepp/quick/elements/RayCaster.h>
#include <QDebug>

namespace three {
namespace quick {

three::Object3D::Ptr GridLines::_create(Scene *scene)
{
  _options.color1.set(_color1.redF(), _color1.greenF(), _color1.blueF());
  _options.color2.set(_color1.redF(), _color1.greenF(), _color1.blueF());
  _grid = helper::Grid::make(_options);
  _scene = scene;

  return _grid;
}

void GridLines::snap(QVariant obj, QVariant pos)
{
  ThreeQObject *object = obj.value<ThreeQObject *>();
  Intersect *intersect = pos.value<Intersect *>();

  if(object && intersect) {
    math::Vector3 pos = intersect->intersection.point + intersect->intersection.face.normal;
    pos = (pos / 50).floor() * 50 + 25;

    QVector3D qpos(pos.x(), abs(pos.y()), pos.z());
    object->setPosition(qpos, true);

    intersect->deleteLater();
  }
}

}
}