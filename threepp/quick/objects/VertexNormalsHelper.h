//
// Created by byter on 2/22/18.
//

#ifndef THREE_PPQ_ObjectHelper_H
#define THREE_PPQ_ObjectHelper_H

#include <QObject>
#include <QColor>

#include <threepp/helper/VertexNormals.h>
#include <threepp/scene/Scene.h>

namespace three {
namespace quick {

class VertexNormalsHelper : public QObject
{
Q_OBJECT
  Q_PROPERTY(unsigned size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(unsigned lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

  QColor _color {"#ff0000"};
  unsigned _size = 1, _lineWidth = 1;
  bool _visible = true;
  bool _configured = false;

  three::helper::VertexNormals::Ptr normalsHelper;

public:
  VertexNormalsHelper(QObject *parent = nullptr) {}

  void create(Object3D::Ptr object, Object3D::Ptr parent, bool force=false)
  {
    if(_configured || force) {
      three::helper::VertexNormalConfig config;
      config.color.r = _color.redF();
      config.color.g = _color.greenF();
      config.color.b = _color.blueF();
      config.lineWidth = _lineWidth;
      config.size = _size;
      normalsHelper = three::helper::VertexNormals::make(object, config);

      parent->add(normalsHelper);
    }
  }

  void setObject(Object3D::Ptr object, Object3D::Ptr parentObject)
  {
    if(normalsHelper) parentObject->remove(normalsHelper);
    create(object, parentObject, true);
  }

  unsigned size() const {return _size;}
  void setSize(unsigned size) {
    if(_size != size) {
      _size = size;

      if(normalsHelper) normalsHelper->setSize(size);
      emit sizeChanged();
    }
    _configured = true;
  }

  unsigned lineWidth() const {return _lineWidth;}
  void setLineWidth(unsigned lineWidth) {
    if(_lineWidth != lineWidth) {
      _lineWidth = lineWidth;

      if(normalsHelper) normalsHelper->setLineWidth(lineWidth);
      emit sizeChanged();
    }
    _configured = true;
  }

  bool visible() const {return _visible;}
  void setVisible(bool visible) {
    if(_visible != visible) {
      _visible = visible;

      if(normalsHelper) normalsHelper->visible() = visible;
      emit visibleChanged();
    }
    _configured = true;
  }

  QColor color() const {return _color;}
  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;

      if(normalsHelper) normalsHelper->setColor(color.redF(), color.greenF(), color.blueF());
      emit colorChanged();
    }
    _configured = true;
  }

  bool configured() const {return _configured;}

signals:
  void sizeChanged();
  void visibleChanged();
  void colorChanged();
  void lineWidthChanged();
};

}
}

#endif //THREE_PPQ_ObjectHelper_H
