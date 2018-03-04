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

class Points : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(float size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(bool sizeAttenuation READ sizeAttenuation WRITE setSizeAttenuation NOTIFY sizeAttenuationChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

protected:
  three::Points::Ptr _points;
  attribute::growing_t<float, math::Vector3> _positions;

  float _size = 1.0f;
  bool _sizeAttenuation = true;
  QColor _color;

  three::PointsMaterial::Ptr _material;


  three::Object3D::Ptr _create(Scene *scene) override
  {
    _points = three::Points::make();

    _points->geometry_t()->setPosition(_positions);
    _points->material<0>()->color = Color(_color.redF(), _color.greenF(), _color.blueF());
    _material = _points->material<0>();

    return _points;
  }

public:
  Points(QObject *parent = nullptr) : ThreeQObject(parent)
  {
    _positions = attribute::growing<float, math::Vector3>();
  }

  const QColor &color()const  {return _color;}

  void setColor(const QColor &color)
  {
    if(_color != color) {
      _color = color;
      if(_material) _material->color = Color(color.redF(), color.greenF(), color.blueF());
      emit colorChanged();
    }
  }

  float size() const {return _size;}

  void setSize(float size) {
    if(_size != size) {
      _size = size;
      if(_material) _material->size = _size;
      emit sizeChanged();
    }
  }

  bool sizeAttenuation() const {return _sizeAttenuation;}

  void setSizeAttenuation(bool sizeAttenuation) {
    if(_sizeAttenuation != sizeAttenuation) {
      _sizeAttenuation = sizeAttenuation;
      if(_material) _material->sizeAttenuation = _sizeAttenuation;
      emit sizeAttenuationChanged();
    }
  }

  Q_INVOKABLE void addPoint(QVector3D point)
  {
    _positions->next() = math::Vector3(point.x(), point.y(), point.z());
  }

signals:
  void colorChanged();
  void sizeChanged();
  void sizeAttenuationChanged();
};

}
}

#endif //THREE_PPQ_POINTS_H
