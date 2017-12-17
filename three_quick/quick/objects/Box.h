//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_QUICK_BOX_H
#define THREEPP_QUICK_BOX_H

#include "quick/ThreeDScene.h"
#include <geometry/Box.h>
#include <material/MeshBasicMaterial.h>
#include <material/MeshLambertMaterial.h>
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

  three::Mesh::Ptr _cube;

protected:
  three::Object3D::Ptr _create(ThreeDScene *scene) override
  {
    geometry::Box::Ptr geometry = geometry::Box::make(_width, _height, _depth);
    three::Material::Ptr mat = material()->create();

    three::MeshBasicMaterial::Ptr mbm = std::dynamic_pointer_cast<three::MeshBasicMaterial>(mat);
    if(mbm) return Mesh_T<geometry::Box, three::MeshBasicMaterial>::make("box", geometry, mbm);

    three::MeshLambertMaterial::Ptr mlm = std::dynamic_pointer_cast<three::MeshLambertMaterial>(mat);
    if(mlm) return Mesh_T<geometry::Box, three::MeshLambertMaterial>::make("box", geometry, mlm);

    return nullptr;
  }

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

signals:
  void widthChanged();
  void heightChanged();
  void depthChanged();
};

}
}

#endif //THREEPP_QUICK_BOX_H
