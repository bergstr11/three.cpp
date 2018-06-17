//
// Created by byter on 6/17/18.
//

#ifndef THREE_PPQ_TEXT3D_H
#define THREE_PPQ_TEXT3D_H

#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/geometry/Text.h>

namespace three {
namespace quick {

class Text3D : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
  Q_PROPERTY(QString font READ font WRITE setFont NOTIFY fontChanged)
  Q_PROPERTY(float size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(unsigned curveSegments READ curveSegments WRITE setCurveSegments NOTIFY curveSegmentsChanged)
  Q_PROPERTY(float bevelThickness READ bevelThickness WRITE setBevelThickness NOTIFY bevelThicknessChanged)
  Q_PROPERTY(float bevelSize READ bevelSize WRITE setBevelSize NOTIFY bevelSizeChanged)
  Q_PROPERTY(bool bevelEnabled READ bevelEnabled WRITE setBevelEnabled NOTIFY bevelEnabledChanged)

  QString _text;
  QString _font;
  float _size = 70;
  float _height = 20;
  unsigned _curveSegments = 4;
  float _bevelThickness = 2;
  float _bevelSize = 1.5;
  bool _bevelEnabled = true;

  MeshCreatorG<geometry::Text> _creator {"text"};

protected:
  three::Object3D::Ptr _create() override;

  void updateMaterial() override {
    material()->identify(_creator);
  }

public:

  QString text() const {return _text;}
  void setText(QString text) {
    if(_text != text) {
      _text = text;
      emit textChanged();
    }
  }

  QString font() const {return _font;}
  void setFont(QString font) {
    if(_font != font) {
      _font = font;
      emit fontChanged();
    }
  }

  float size() const {return _size;}
  void setSize(float size) {
    if(_size != size) {
      _size = size;
      emit sizeChanged();
    }
  }

  float height() const {return _height;}
  void setHeight(float height) {
    if(_height != height) {
      _height = height;
      emit heightChanged();
    }
  }

  unsigned curveSegments() const {return _curveSegments;}
  void setCurveSegments(unsigned curveSegments) {
    if(_curveSegments != curveSegments) {
      _curveSegments = curveSegments;
      emit curveSegmentsChanged();
    }
  }

  float bevelThickness() const {return _bevelThickness;}
  void setBevelThickness(float bevelThickness) {
    if(_bevelThickness != bevelThickness) {
      _bevelThickness = bevelThickness;
      emit bevelThicknessChanged();
    }
  }

  float bevelSize() const {return _bevelSize;}
  void setBevelSize(float bevelSize) {
    if(_bevelSize != bevelSize) {
      _bevelSize = bevelSize;
      emit bevelSizeChanged();
    }
  }

  bool bevelEnabled() const {return _bevelEnabled;}
  void setBevelEnabled(bool bevelEnabled) {
    if(_bevelEnabled != bevelEnabled) {
      _bevelEnabled = bevelEnabled;
      emit bevelEnabledChanged();
    }
  }

signals:
  void textChanged();
  void fontChanged();
  void sizeChanged();
  void heightChanged();
  void curveSegmentsChanged();
  void bevelThicknessChanged();
  void bevelSizeChanged();
  void bevelEnabledChanged();
};

}
}
#endif //THREE_PPQ_TEXT3D_H
