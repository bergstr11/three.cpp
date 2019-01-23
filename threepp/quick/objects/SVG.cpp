//
// Created by byter on 6/18/18.
//

#include "SVG.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QColor>

#include <threepp/extras/SVG.h>
#include <threepp/geometry/Shape.h>
#include <threepp/geometry/Extrude.h>

namespace three {
namespace quick {

void SVG::append_material(QQmlListProperty<Material> *list, Material *obj)
{
  SVG *item = qobject_cast<SVG *>(list->object);
  if (item) item->_materials.append(obj);
}
int SVG::count_materials(QQmlListProperty<Material> *list)
{
  SVG *item = qobject_cast<SVG *>(list->object);
  return item ? item->_materials.size() : 0;
}
Material *SVG::material_at(QQmlListProperty<Material> *list, int index)
{
  SVG *item = qobject_cast<SVG *>(list->object);
  return item ? item->_materials.at(index) : nullptr;
}
void SVG::clear_materials(QQmlListProperty<Material> *list)
{
  SVG *item = qobject_cast<SVG *>(list->object);
  if(item) item->_materials.clear();
}

QQmlListProperty<Material> SVG::materials()
{
  return QQmlListProperty<Material>(this, nullptr,
                                    &SVG::append_material,
                                    &SVG::count_materials,
                                    &SVG::material_at,
                                    &SVG::clear_materials);
}

three::Object3D::Ptr SVG::_create()
{
  if(_file.isEmpty()) return nullptr;

  QFile file(_file);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCritical() << "cannot access svg file" << _file;
    return nullptr;
  }
  extras::SVG svg = extras::SVG::fromXML(file.readAll().data(), file.size());

  geometry::ExtrudeOptions options;
  options.bevelThickness = _bevelThickness;
  options.bevelSize = _bevelSize;
  options.bevelEnabled = _bevelEnabled;
  options.depth = _extrudeDepth;

  std::vector<extras::Shape::Ptr> allShapes;
  unsigned groupStart = 0, groupIndex = 0;
  const auto &paths = svg.paths();
  for(const auto &shapePath : paths) {
    std::vector<extras::Shape::Ptr> shapes = shapePath.toShapes();
    allShapes.insert(allShapes.cbegin(), shapes.begin(), shapes.end());

    if(_useShapeGroups) {
      options.shapeGroups.emplace_back(groupIndex, groupStart, shapes.size());
      groupStart += shapes.size();
      groupIndex++;
    }
  }
  geometry::Extrude::Ptr geometry = geometry::Extrude::make(allShapes, options);
  geometry->setCenter();

  _mesh = DynamicMesh::make(geometry);

  for(unsigned i=0; i < _materials.size(); i++) {
    if(_useSvgColors && i < paths.size()) {
      const auto &color = paths[i].color;
      _materials[i]->applyColor(QColor(color.r, color.g, color.b));
    }
    _mesh->addMaterial(_materials[i]->getMaterial());
  }

  return _mesh;
}

three::Object3D::Ptr SVG::_copy(Object3D::Ptr copyable)
{
  DynamicMesh *mesh = copyable->typer;

  _mesh = DynamicMesh::Ptr(mesh->cloned());

  return _mesh;
}

void SVG::setFile(QString file) {
  if(_file != file) {
    _file = file;
    emit fileChanged();
  }
}

void SVG::setBevelThickness(float bevelThickness) {
  if(_bevelThickness != bevelThickness) {
    _bevelThickness = bevelThickness;
    recreate();
    emit bevelThicknessChanged();
  }
}

void SVG::setBevelSize(float bevelSize) {
  if(_bevelSize != bevelSize) {
    _bevelSize = bevelSize;
    recreate();
    emit bevelSizeChanged();
  }
}

void SVG::setUseSvgColors(bool useSvgColors)
{
  if(_useSvgColors != useSvgColors) {
    _useSvgColors = useSvgColors;
    recreate();
    emit useSvgColorsChanged();
  }
}

void SVG::setBevelEnabled(bool bevelEnabled) {
  if(_bevelEnabled != bevelEnabled) {
    _bevelEnabled = bevelEnabled;
    recreate();
    emit bevelEnabledChanged();
  }
}

void SVG::setUseShapeGroups(bool useShapeGroups) {
  if(_useShapeGroups != useShapeGroups) {
    _useShapeGroups = useShapeGroups;
    recreate();
    emit useShapeGroupsChanged();
  }
}

void SVG::setExtrudeDepth(float extrudeDepth)
{
  if(_extrudeDepth != extrudeDepth) {
    _extrudeDepth = extrudeDepth;
    recreate();
    emit extrudeDepthChanged();
  }
}

}
}
