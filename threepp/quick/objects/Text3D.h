//
// Created by byter on 6/17/18.
//

#ifndef THREE_PPQ_TEXT3D_H
#define THREE_PPQ_TEXT3D_H

#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/geometry/Text.h>
#include <threepp/objects/Mesh.h>

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
  Q_PROPERTY(QQmlListProperty<three::quick::Material> materials READ materials)

  QString _text;
  QString _font;
  float _size = 70;
  float _height = 20;
  unsigned _curveSegments = 4;
  float _bevelThickness = 2;
  float _bevelSize = 1.5;
  bool _bevelEnabled = true;

  QList<Material *> _materials;

  static void append_material(QQmlListProperty<Material> *list, Material *obj);
  static int count_materials(QQmlListProperty<Material> *);
  static Material *material_at(QQmlListProperty<Material> *, int);
  static void clear_materials(QQmlListProperty<Material> *);

  QQmlListProperty<Material> materials();

  DynamicMesh::Ptr _mesh;

protected:
  three::Object3D::Ptr _copy(Object3D::Ptr copyable) override;
  three::Object3D::Ptr _create() override;

  void updateMaterial() override {
    if(_mesh) _mesh->setMaterial(material()->getMaterial());
  }

public:
  QString text() const {return _text;}
  void setText(QString text);

  QString font() const {return _font;}
  void setFont(QString font);

  float size() const {return _size;}
  void setSize(float size);

  float height() const {return _height;}
  void setHeight(float height);

  unsigned curveSegments() const {return _curveSegments;}
  void setCurveSegments(unsigned curveSegments);

  float bevelThickness() const {return _bevelThickness;}
  void setBevelThickness(float bevelThickness);

  float bevelSize() const {return _bevelSize;}
  void setBevelSize(float bevelSize);

  bool bevelEnabled() const {return _bevelEnabled;}
  void setBevelEnabled(bool bevelEnabled);

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
