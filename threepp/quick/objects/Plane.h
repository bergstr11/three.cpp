//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_QUICK_PLANE_H
#define THREEPPQ_QUICK_PLANE_H

#include <threepp/quick/scene/Scene.h>
#include <threepp/geometry/Plane.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/objects/Mesh.h>
#include "threepp/quick/qutil/MeshCreator.h"

namespace three {
namespace quick {

class Plane : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(unsigned height READ height WRITE setHeight NOTIFY heightChanged)

  unsigned _width=1, _height=1;

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

signals:
  void widthChanged();
  void heightChanged();
};

}
}

#endif //THREEPPQ_QUICK_PLANE_H
