//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_LINE_H
#define THREE_QT_LINE_H

#include <core/Object3D.h>
#include <material/Material.h>
#include <core/Raycaster.h>
#include <material/LineMaterial.h>

namespace three {

class Line : public Object3D_GM<BufferGeometry, LineBasicMaterial>
{
  const unsigned _steps;

protected:
  Line(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material, unsigned steps)
     : Object3D(object::ResolverT<Line>::make(*this)),
       Object3D_GM(geometry, nullptr, material), _steps(steps) {}

  Line(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material) : Line(geometry, material, 1) {}

public:
  using Ptr = std::shared_ptr<Line>;

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;

  bool renderable() const override {return true;}

  unsigned steps() const {return _steps;}
};

class LineSegments : public Line
{
protected:
  LineSegments(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material)
     : Object3D(object::ResolverT<LineSegments>::make(*this)), Line(geometry, material, 2) {}
};

}
#endif //THREE_QT_LINE_H
