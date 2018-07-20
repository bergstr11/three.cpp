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

namespace three {
namespace quick {

class Plane : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(unsigned widthSegments READ widthSegments WRITE setWidthSegments NOTIFY widthSegmentsChanged)
  Q_PROPERTY(unsigned heightSegments READ heightSegments WRITE setHeightSegments NOTIFY heightSegmentsChanged)

  float _width=1, _height=1;
  unsigned _widthSegments=1, _heightSegments=1;

  DynamicMesh::Ptr _mesh;

protected:
  three::Object3D::Ptr _create() override
  {
    switch(_geometryType) {
      case Three::LinearGeometry: {
        _mesh = DynamicMesh::make(geometry::Plane::make(_width, _height, _widthSegments, _heightSegments));
        break;
      }
      case Three::BufferGeometry: {
        _mesh = DynamicMesh::make(geometry::buffer::Plane::make(_width, _height, _widthSegments, _heightSegments));
        break;
      }
    }

    _mesh->setMaterial(material()->getMaterial());

    return _mesh;
  }

  void updateMaterial() override {
    _mesh->setMaterial(material()->getMaterial());
  }

public:
  Plane(QObject *parent = nullptr) : ThreeQObject(parent) {}

  float width() const {return _width;}
  void setWidth(float width) {
    if(_width != width) {
      _width = width;
      emit widthChanged();
    }
  }

  float height() const {return _height;}
  void setHeight(float height) {
    if(_height != height) {
      _height = height;
      emit heightChanged();
    }
  }

  unsigned widthSegments() const {return _widthSegments;}
  void setWidthSegments(unsigned segments) {
    if(_widthSegments != segments) {
      _widthSegments = segments;
      emit widthSegmentsChanged();
    }
  }

  unsigned heightSegments() const {return _heightSegments;}
  void setHeightSegments(unsigned segments) {
    if(_heightSegments != segments) {
      _heightSegments = segments;
      emit heightSegmentsChanged();
    }
  }

signals:
  void widthChanged();
  void heightChanged();
  void widthSegmentsChanged();
  void heightSegmentsChanged();
};

}
}

#endif //THREEPPQ_QUICK_PLANE_H
