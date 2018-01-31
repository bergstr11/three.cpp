//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_GridLines_H
#define THREEPP_GridLines_H

#include "quick/scene/Scene.h"
#include <helper/Grid.h>

namespace three {
namespace quick {

class Intersect;

class GridLines : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(unsigned divisions READ divisions WRITE setDivisions NOTIFY divisionsChanged)
  Q_PROPERTY(QColor color1 READ color1 WRITE setColor1 NOTIFY color1Changed)
  Q_PROPERTY(QColor color2 READ color2 WRITE setColor2 NOTIFY color2Changed)

  helper::Grid::Options _options;
  QColor _color1, _color2;
  helper::Grid::Ptr _grid;

  Scene *_scene = nullptr;

protected:
  three::Object3D::Ptr _create(Scene *scene) override;

public:
  GridLines(QObject *parent = nullptr)
     : ThreeQObject(parent),
       _color1(QColor::fromRgbF(_options.color1.r, _options.color1.g, _options.color1.b)),
       _color2(QColor::fromRgbF(_options.color2.r, _options.color2.g, _options.color2.b))
  {}

  unsigned size() {return _options.size;}

  void setSize(unsigned size) {
    if(_options.size != size) {
      _options.size = size;
      emit sizeChanged();
    }
  }

  unsigned divisions() {return _options.divisions;}

  void setDivisions(unsigned divisions) {
    if(_options.divisions != divisions) {
      _options.divisions = divisions;
      emit divisionsChanged();
    }
  }

  const QColor &color1() const {return _color1;}

  void setColor1(const QColor &color) {
    if(_color1 != color) {
      _color1 = color;
      emit color1Changed();
    }
  }

  const QColor &color2() {return _color2;}

  void setColor2(const QColor &color) {
    if(_color2 != color) {
      _color2 = color;
      emit color2Changed();
    }
  }

  Q_INVOKABLE void snap(QVariant object, QVariant pos);

signals:
  void sizeChanged();
  void divisionsChanged();
  void color1Changed();
  void color2Changed();
};

}
}

#endif //THREEPP_GridLines_H
