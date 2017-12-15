//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_QUICK_BOX_H
#define THREEPP_QUICK_BOX_H

#include "quick/ThreeDScene.h"
#include <geometry/Box.h>
#include <material/MeshBasicMaterial.h>
#include <objects/Mesh.h>

namespace three {
namespace quick {

class Box : public ThreeDObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(unsigned height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(unsigned depth READ depth WRITE setDepth NOTIFY depthChanged)

  unsigned _width=0, _height=0, _depth=0;

public:

  unsigned width() const {return _width;}
  unsigned height() const {return _height;}
  unsigned depth() const {return _depth;}

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

  void setDepth(unsigned depth) {
    if(_depth != depth) {
      _depth = depth;
      emit depthChanged();
    }
  }

  three::Object3D::Ptr create() override
  {
    geometry::Box::Ptr geometry = geometry::Box::make(_width, _height, _depth);
    three::Material::Ptr mat = material()->create();

    three::MeshBasicMaterial::Ptr mbm = std::dynamic_pointer_cast<three::MeshBasicMaterial>(mat);
    if(mbm) {
      three::Mesh::Ptr cube = Mesh_T<geometry::Box, three::MeshBasicMaterial>::make("box", geometry, mbm);
      cube->rotation().setX(_rotation.x());
      cube->position().set(_position.x(), _position.y(), _position.z());

      return cube;
    }
    return nullptr;
  }

signals:
  void widthChanged();
  void heightChanged();
  void depthChanged();
};

}
}

#endif //THREEPP_QUICK_BOX_H
