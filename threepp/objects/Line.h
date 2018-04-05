//
// Created by byter on 22.08.17.
//

#ifndef THREEPP_LINE_H
#define THREEPP_LINE_H

#include <threepp/core/Object3D.h>
#include <threepp/core/Raycaster.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/material/Material.h>
#include <threepp/material/LineMaterial.h>

namespace three {

class Line : public Object3D_GM<BufferGeometry, LineBasicMaterial>
{
  const unsigned _steps;

protected:
  Line(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material, unsigned steps=1)
     : Object3D(object::ResolverT<Line>::make(*this)),
       Object3D_GM(geometry, nullptr, material), _steps(steps) {}

  Line(const Line &line)
     : Object3D(object::ResolverT<Line>::make(*this)),
       Object3D_GM(BufferGeometry::Ptr(geometry_t()->cloned()), nullptr, LineBasicMaterial::Ptr(line.material<0>()->cloned())),
       _steps(line._steps) {}

public:
  using Ptr = std::shared_ptr<Line>;
  static Ptr make(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material) {
    return Ptr(new Line(geometry, material));
  }
  static Ptr make(const std::string &name, BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material) {
    auto ptr = Ptr(new Line(geometry, material));
    ptr->setName(name);
    return ptr;
  }

  void raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects) const override;

  bool isShadowRenderable() const override {return true;}

  unsigned steps() const {return _steps;}

  Line *cloned() const override
  {
    return new Line(*this);
  }
};

class LineSegments : public Line
{
protected:
  LineSegments(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material)
     : Object3D(object::ResolverT<LineSegments>::make(*this)), Line(geometry, material, 2) {}

  LineSegments(const LineSegments &segments)
     : Object3D(object::ResolverT<LineSegments>::make(*this)),
       Line(segments) {}

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

  LineSegments *cloned() const override
  {
    return new LineSegments(*this);
  }
};

}
#endif //THREEPP_LINE_H
