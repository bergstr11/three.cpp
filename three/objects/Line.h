//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_LINE_H
#define THREE_QT_LINE_H

#include <core/Object3D.h>
#include <core/Raycaster.h>
#include <core/BufferGeometry.h>
#include <material/Material.h>
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

public:
  using Ptr = std::shared_ptr<LineSegments>;
  static Ptr make(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material) {
    return Ptr(new LineSegments(geometry, material));
  }
  static Ptr make(const char *name, BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material) {
    Ptr p(new LineSegments(geometry, material));
    p->_name = name;
    return p;
  }
};

}
#endif //THREE_QT_LINE_H
