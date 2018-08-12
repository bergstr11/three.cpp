//
// Created by byter on 6/17/18.
//

#ifndef THREE_PPQ_SVG_H
#define THREE_PPQ_SVG_H

#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/objects/Mesh.h>

namespace three {
namespace quick {

class SVG : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
  Q_PROPERTY(float depth READ extrudeDepth WRITE setExtrudeDepth NOTIFY extrudeDepthChanged)
  Q_PROPERTY(float bevelThickness READ bevelThickness WRITE setBevelThickness NOTIFY bevelThicknessChanged)
  Q_PROPERTY(float bevelSize READ bevelSize WRITE setBevelSize NOTIFY bevelSizeChanged)
  Q_PROPERTY(bool bevelEnabled READ bevelEnabled WRITE setBevelEnabled NOTIFY bevelEnabledChanged)
  Q_PROPERTY(bool useSvgColors READ useSvgColors WRITE setUseSvgColors NOTIFY useSvgColorsChanged)
  Q_PROPERTY(bool useShapeGroups READ useShapeGroups WRITE setUseShapeGroups NOTIFY useShapeGroupsChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::Material> materials READ materials)

  QString _file;
  float _bevelThickness = 2;
  float _bevelSize = 1.5;
  bool _bevelEnabled = true;
  bool _useSvgColors = true;
  bool _useShapeGroups = false;
  float _extrudeDepth = 100;

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
  QString file() const {return _file;}
  void setFile(QString file);

  float bevelThickness() const {return _bevelThickness;}
  void setBevelThickness(float bevelThickness);

  float bevelSize() const {return _bevelSize;}
  void setBevelSize(float bevelSize);

  bool bevelEnabled() const {return _bevelEnabled;}
  void setBevelEnabled(bool bevelEnabled);

  bool useSvgColors() const {return _useSvgColors;}
  void setUseSvgColors(bool useSvgColors);

  bool useShapeGroups() const {return _useShapeGroups;}
  void setUseShapeGroups(bool useShapeGroups);

  float extrudeDepth() const {return _extrudeDepth;}
  void setExtrudeDepth(float extrudeDepth);

signals:
  void fileChanged();
  void bevelThicknessChanged();
  void bevelSizeChanged();
  void bevelEnabledChanged();
  void extrudeDepthChanged();
  void useSvgColorsChanged();
  void useShapeGroupsChanged();
};

}
}
#endif //THREE_PPQ_SVG_H
