//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_QUICK_BOX_H
#define THREEPPQ_QUICK_BOX_H

#include <threepp/quick/scene/Scene.h>
#include <threepp/geometry/Box.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/objects/Mesh.h>

namespace three {
namespace quick {

class Box : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(unsigned height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(unsigned depth READ depth WRITE setDepth NOTIFY depthChanged)

  unsigned _width=0, _height=0, _depth=0;

  DynamicMesh::Ptr _mesh;

protected:
  three::Object3D::Ptr _create() override
  {
    _mesh = DynamicMesh::make(geometry::Box::make(_width, _height, _depth));
    _mesh->setMaterial(material()->getMaterial());

    return _mesh;
  }

  void updateMaterial() override {
    _mesh->setMaterial(material()->getMaterial());
  }

public:
  Box(QObject *parent = nullptr) : ThreeQObject(parent) {}

  unsigned width() const {return _width;}
  unsigned height() const {return _height;}
  unsigned depth() const {return _depth;}

  void setWidth(unsigned width) {
    if(_width != width) {
      _width = width;
      if(_mesh) _mesh->geometry_t<geometry::Box>()->setWidth(width);
      emit widthChanged();
    }
  }

  void setHeight(unsigned height) {
    if(_height != height) {
      _height = height;
      if(_mesh) _mesh->geometry_t<geometry::Box>()->setHeight(height);
      emit heightChanged();
    }
  }

  void setDepth(unsigned depth) {
    if(_depth != depth) {
      _depth = depth;
      if(_mesh) _mesh->geometry_t<geometry::Box>()->setDepth(depth);
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

#endif //THREEPPQ_QUICK_BOX_H
