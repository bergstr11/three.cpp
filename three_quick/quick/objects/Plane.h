//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_QUICK_PLANE_H
#define THREEPP_QUICK_PLANE_H

#include "quick/ThreeDScene.h"
#include <geometry/Plane.h>
#include <material/MeshBasicMaterial.h>
#include <objects/Mesh.h>

namespace three {
namespace quick {

class Plane : public ThreeDObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(unsigned height READ height WRITE setHeight NOTIFY heightChanged)

  unsigned _width=1, _height=1;

  three::Mesh::Ptr _plane;

public:

  unsigned width() const {return _width;}
  unsigned height() const {return _height;}

  void setWidth(unsigned width) {
    if(_width != width) {
      _width = width;
      emit widthChanged();
    }
  }
  void setHeight(unsigned height) {
    if(_height != height) {
      _height = height;
      emit heightChanged();
    }
  }

  three::Object3D::Ptr create() override
  {
    geometry::Plane::Ptr planeGeometry = geometry::Plane::make(_width, _height, 1, 1);
    three::Material::Ptr mat = material()->create();

    three::MeshBasicMaterial::Ptr mbm = std::dynamic_pointer_cast<three::MeshBasicMaterial>(mat);
    if(mbm) {
      _plane = Mesh_T<geometry::Plane, three::MeshBasicMaterial>::make("plane", planeGeometry, mbm);
      _plane->rotation().setX(_rotation.x());
      _plane->position().set(_position.x(), _position.y(), _position.z());

      return _plane;
    }
    return nullptr;
  }

signals:
  void widthChanged();
  void heightChanged();
};

}
}

#endif //THREEPP_QUICK_PLANE_H
