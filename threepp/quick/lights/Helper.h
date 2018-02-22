//
// Created by byter on 2/22/18.
//

#ifndef THREE_PP_HELPER_H
#define THREE_PP_HELPER_H

#include <QObject>

namespace three {
namespace quick {

class Helper : public QObject
{
Q_OBJECT
  Q_PROPERTY(unsigned size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

  QColor _color;
  unsigned _size = 0;
  bool _visible = true;
  bool _configured;

public:
  Helper(QObject *parent = nullptr) {}

  unsigned size() const {return _size;}
  void setSize(unsigned size) {
    if(_size != size) {
      _size = size;
      emit sizeChanged();
    }
    _configured = true;
  }

  bool visible() const {return _visible;}
  void setVisible(bool visible) {
    if(_visible != visible) {
      _visible = visible;
      emit visibleChanged();
    }
    _configured = true;
  }

  QColor color() const {return _color;}
  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  bool configured() const {return _configured;}

signals:
  void sizeChanged();
  void visibleChanged();
  void colorChanged();
};

}
}

#endif //THREE_PP_HELPER_H
