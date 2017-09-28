//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_LINE_H
#define THREE_QT_LINE_H

#include <core/Object3D.h>
#include <material/Material.h>
#include <core/Raycaster.h>

namespace three {

class Line : public Object3D
{
  Geometry::Ptr _geometry;
  Material::Ptr _material;

  const unsigned _steps;

protected:
  Line(Geometry::Ptr geometry, Material::Ptr material, unsigned steps) : _steps(steps), _geometry(geometry), _material(material) {}

public:
  Line(Geometry::Ptr geometry, Material::Ptr material) : Line(geometry, material, 1) {}

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;

  bool renderable() const override {return true;}

  unsigned steps() const {return _steps;}
};

class LineSegments : public Line
{
public:
  LineSegments(Geometry::Ptr geometry, Material::Ptr material) : Line(geometry, material, 2) {}
};

}
#endif //THREE_QT_LINE_H
