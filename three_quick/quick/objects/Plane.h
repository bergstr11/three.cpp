//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_QUICK_PLANE_H
#define THREEPP_QUICK_PLANE_H

#include "quick/scene/Scene.h"
#include <geometry/Plane.h>
#include <material/MeshBasicMaterial.h>
#include <material/MeshLambertMaterial.h>
#include <objects/Mesh.h>
#include "MeshCreator.h"
#include "../Three.h"

namespace three {
namespace quick {

class Plane : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(unsigned height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(three::quick::Three::GeometryType type READ geometryType WRITE setGeometryType NOTIFY geometryTypeChanged)

  unsigned _width=1, _height=1;

  Three::GeometryType _geometryType = Three::DefaultGeometry;

  MeshCreator::Ptr _creator;

protected:
  three::Object3D::Ptr _create(Scene *scene) override
  {
    switch(_geometryType) {
      case Three::DefaultGeometry: {
        auto creator = MeshCreatorG<geometry::Plane>::make("plane");
        creator->set(geometry::Plane::make(_width, _height, 1, 1));
        _creator = creator;
        break;
      }
      case Three::BufferGeometry: {
        auto creator = MeshCreatorG<geometry::buffer::Plane>::make("plane");
        creator->set(geometry::buffer::Plane::make(_width, _height, 1, 1));
        _creator = creator;
        break;
      }
    }

    material()->identify(*_creator);

    return _creator->getMesh();
  }

  void updateMaterial() override {
    material()->identify(*_creator);
  }

public:
  Plane(QObject *parent = nullptr) : ThreeQObject(parent) {}

  unsigned width() const {return _width;}
  void setWidth(unsigned width) {
    if(_width != width) {
      _width = width;
      emit widthChanged();
    }
  }

  unsigned height() const {return _height;}
  void setHeight(unsigned height) {
    if(_height != height) {
      _height = height;
      emit heightChanged();
    }
  }

  Three::GeometryType geometryType() const {return _geometryType;}
  void setGeometryType(Three::GeometryType geometryType) {
    if(_geometryType != geometryType) {
      _geometryType = geometryType;
      emit geometryTypeChanged();
    }
  }

signals:
  void widthChanged();
  void heightChanged();
  void geometryTypeChanged();
};

}
}

#endif //THREEPP_QUICK_PLANE_H
