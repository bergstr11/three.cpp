//
// Created by byter on 22.08.17.
//

#ifndef THREEPP_LINE_H
#define THREEPP_LINE_H

#include <threepp/util/osdecl.h>
#include <threepp/core/Object3D.h>
#include <threepp/core/Raycaster.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/material/Material.h>
#include <threepp/material/LineMaterial.h>

namespace three {

class DLX Line : public Object3D
{
  const unsigned _steps;

protected:
  Line(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material, unsigned steps, const object::Typer &typer)
     : Object3D(geometry, material), _steps(steps)
  {
    Object3D::typer = typer;
    Object3D::typer.allow<Line>();
  }

  Line(const Line &line, const object::Typer &typer) : Object3D(line), _steps(line._steps)
  {
    Object3D::typer = typer;
    Object3D::typer.allow<Line>();
  }

  Line(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material, unsigned steps)
     : Object3D(geometry, material), _steps(steps)
  {
    Object3D::typer = object::Typer(this);
  }

  Line(const Line &line) : Object3D(line), _steps(line._steps)
  {
    Object3D::typer = object::Typer(this);
  }

public:
  using Ptr = std::shared_ptr<Line>;
  static Ptr make(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material, unsigned steps=1) {
    return Ptr(new Line(geometry, material, steps));
  }
  static Ptr make(const std::string &name, BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material, unsigned steps=1) {
    auto ptr = Ptr(new Line(geometry, material, steps));
    ptr->setName(name);
    return ptr;
  }

  void raycast(const Raycaster &raycaster, IntersectList &intersects) override;

  bool isShadowRenderable() const override {return true;}

  unsigned steps() const {return _steps;}

  BufferGeometry *bufferGeometry() const {
    return (BufferGeometry *) _geometry->typer;
  }

  LineBasicMaterial *lineMaterial() const {
    return (LineBasicMaterial *)_materials[0]->typer;
  }

  Line *cloned() const override
  {
    return new Line(*this);
  }
};

class DLX LineSegments : public Line
{
protected:
  LineSegments(BufferGeometry::Ptr geometry, LineBasicMaterial::Ptr material)
     : Line(geometry, material, 2, object::Typer(this))
  {
    typer.allow<Line>();
  }

  LineSegments(const LineSegments &segments)
     : Line(segments, object::Typer(this))
  {
    typer.allow<Line>();
  }

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
