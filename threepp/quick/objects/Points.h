//
// Created by byter on 2/26/18.
//

#ifndef THREE_PPQ_POINTS_H
#define THREE_PPQ_POINTS_H

#include <QVector3D>
#include <QList>
#include <threepp/quick/scene/Scene.h>
#include <threepp/objects/Points.h>

namespace three {
namespace quick {

class PointsMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

  QColor _color;
  three::PointsMaterial::Ptr _material;

public:
  explicit PointsMaterial(ThreeQObject *parent=nullptr): Material(parent) {}

  const QColor &color()const  {return _color;}

  void setColor(const QColor &color)
  {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  void addTo(three::quick::ObjectRootContainer*) override {}

  three::Material::Ptr material() const override {
    return _material;
  }

signals:
  void colorChanged();
};

class Points : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(PointsMaterial * material READ material)

protected:
  three::Points::Ptr _points;
  attribute::growing_t<float, math::Vector3> _positions;

  PointsMaterial _material;

  QList<QVector3D> _geomPoints;


  three::Object3D::Ptr _create(Scene *scene) override
  {
    _points = three::Points::make();

    _positions = attribute::growing<float, math::Vector3>();
    for(const auto &gp : _geomPoints)
      _positions->next() = math::Vector3(gp.x(), gp.y(), gp.z());

    _points->buffergeometry()->setPosition(_positions);

    return _points;
  }

public:
  Points(QObject *parent = nullptr) : ThreeQObject(parent) {}

  PointsMaterial * material() {return &_material;}

  Q_INVOKABLE void addPoint(QVector3D point)
  {
    if(_points) {
      _positions->next() = math::Vector3(point.x(), point.y(), point.z());
    }
    else {
      _geomPoints.append(point);
    }
  }
};

}
}

#endif //THREE_PPQ_POINTS_H
