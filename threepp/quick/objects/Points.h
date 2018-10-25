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

  three::Object3D::Ptr _create() override
  {
    _points = three::Points::make(
       BufferGeometry::make(_positions),
       PointsMaterial::make(Color(_color.redF(), _color.greenF(), _color.blueF())));

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
      if(_points) _points->pointsMaterial()->color = Color(color.redF(), color.greenF(), color.blueF());
      emit colorChanged();
    }
  }

  float size() const {return _size;}

  void setSize(float size) {
    if(_size != size) {
      _size = size;
      if(_points) _points->pointsMaterial()->size = _size;
      emit sizeChanged();
    }
  }

  bool sizeAttenuation() const {return _sizeAttenuation;}

  void setSizeAttenuation(bool sizeAttenuation) {
    if(_sizeAttenuation != sizeAttenuation) {
      _sizeAttenuation = sizeAttenuation;
      if(_points) _points->pointsMaterial()->sizeAttenuation = _sizeAttenuation;
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
