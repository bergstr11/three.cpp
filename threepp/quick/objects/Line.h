//
// Created by byter on 4/5/18.
//

#ifndef THREE_PP_LINE_H
#define THREE_PP_LINE_H

#include <threepp/objects/Line.h>
#include <threepp/quick/objects/ThreeQObject.h>

namespace three {
namespace quick {

class Line : public ThreeQObject
{
Q_OBJECT
public:
  enum LineCap {roundCap=(int)three::LineCap::round};
  enum LineJoin {roundJoin=(int)three::LineJoin::round};
  Q_ENUM(LineCap)
  Q_ENUM(LineJoin)

private:
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(float length READ length WRITE setLength NOTIFY lengthChanged)
  Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged)
  Q_PROPERTY(LineCap lineCap READ lineCap WRITE setLineCap NOTIFY lineCapChanged)
  Q_PROPERTY(LineJoin lineJoin READ lineJoin WRITE setLineJoin NOTIFY lineJoinChanged)

  QColor _color;
  LineCap _lineCap = roundCap;
  LineJoin _lineJoin = roundJoin;

  QVector3D _direction;
  float _width = 1, _length = 0;

  three::Line::Ptr _line;

protected:
  three::Object3D::Ptr _create() override
  {
    BufferGeometry::Ptr geometry = BufferGeometry::make();

    math::Vector3 dir(_direction.x(), _direction.y(), _direction.z());
    dir *= _length;
    dir += math::Vector3(_position.x(), _position.y(), _position.z());

    auto vertices = attribute::prealloc<float, Vertex>(2);
    vertices->next() = {0.0f, 0.0f, 0.0f};
    vertices->next() = dir;
    geometry->setPosition(vertices);

    auto colors = attribute::prealloc<float, Color>(2);
    colors->next() = Color(_color.redF(), _color.greenF(), _color.blueF());
    colors->next() = Color(_color.redF(), _color.greenF(), _color.blueF());
    geometry->setColor(colors);

    auto material = LineBasicMaterial::make(Colors::Vertex);
    material->linewidth = _width;
    material->linecap = (three::LineCap)_lineCap;
    material->linejoin = (three::LineJoin)_lineJoin;
    material->linewidth = _width;

    _line = three::Line::make(geometry, material);
    return _line;
  }

public:
  Line(QObject *parent = nullptr) : ThreeQObject(parent) {}

  const QColor &color() const {return _color;}
  void setColor(const QColor &color) {
    if(color != _color) {
      _color = color;
      emit colorChanged();
    }
  }

  float width() const {return _width;}
  void setWidth(float lineWidth) {
    if(_width != lineWidth) {
      _width = lineWidth;
      emit widthChanged();
    }
  }

  float length() const {return _length;}
  void setLength(float length) {
    if(_length != length) {
      _length = length;
      emit lengthChanged();
    }
  }

  const QVector3D &direction() {return _direction;}
  void setDirection(const QVector3D &direction) {
    if(_direction != direction) {
      _direction = direction;
      emit directionChanged();
    }
  }

  LineCap lineCap() {return _lineCap;}
  void setLineCap(LineCap lineCap) {
    if(_lineCap != lineCap) {
      _lineCap = lineCap;
      emit lineCapChanged();
    }
  }

  LineJoin lineJoin() {return _lineJoin;}
  void setLineJoin(LineJoin lineJoin) {
    if(_lineJoin != lineJoin) {
      _lineJoin = lineJoin;
      emit lineJoinChanged();
    }
  }

signals:
  void colorChanged();
  void widthChanged();
  void lengthChanged();
  void directionChanged();
  void lineCapChanged();
  void lineJoinChanged();
};

}
}

#endif //THREE_PP_LINE_H
