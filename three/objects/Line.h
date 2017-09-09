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

public:
  Line(Geometry::Ptr geometry, Material::Ptr material) : _geometry(geometry), _material(material) {}

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) override;
};

}
#endif //THREE_QT_LINE_H
