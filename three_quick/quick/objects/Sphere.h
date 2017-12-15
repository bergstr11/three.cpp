//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_QUICK_SPHERE_H
#define THREEPP_QUICK_SPHERE_H

#include "quick/ThreeDScene.h"
#include <geometry/Sphere.h>
#include <material/MeshBasicMaterial.h>
#include <objects/Mesh.h>

namespace three {
namespace quick {

class Sphere : public ThreeDObject
{
  Q_OBJECT
  Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(unsigned widthSegments READ widthSegments WRITE setWidthSegments NOTIFY widthSegmentsChanged)
  Q_PROPERTY(unsigned heightSegments READ heightSegments WRITE setHeightSegments NOTIFY heightSegmentsChanged)

  qreal _radius = 1;
  unsigned _widthSegments=1, _heightSegments=1;

  three::Mesh::Ptr _sphere;

public:

  qreal radius() {return _radius;}
  unsigned widthSegments() const {return _widthSegments;}
  unsigned heightSegments() const {return _heightSegments;}

  void setRadius(qreal radius) {
    if(_radius != radius) {
      _radius = radius;
      emit radiusChanged();
    }
  }
  void setWidthSegments(unsigned widthSegments) {
    if(_widthSegments != widthSegments) {
      _widthSegments = widthSegments;
      emit widthSegmentsChanged();
    }
  }
  void setHeightSegments(unsigned heightSegments) {
    if(_heightSegments != heightSegments) {
      _heightSegments = heightSegments;
      emit heightSegmentsChanged();
    }
  }

  three::Object3D::Ptr create() override
  {
    geometry::Sphere::Ptr geometry = geometry::Sphere::make(_radius, _widthSegments, _heightSegments);
    three::Material::Ptr mat = material()->create();

    three::MeshBasicMaterial::Ptr mbm = std::dynamic_pointer_cast<three::MeshBasicMaterial>(mat);
    if(mbm) {
      _sphere = Mesh_T<geometry::Sphere, three::MeshBasicMaterial>::make("sphere", geometry, mbm);
      _sphere->rotation().setX(_rotation.x());
      _sphere->position().set(_position.x(), _position.y(), _position.z());

      return _sphere;
    }
    return nullptr;
  }

signals:
  void widthSegmentsChanged();
  void heightSegmentsChanged();
  void radiusChanged();
};

}
}

#endif //THREEPP_QUICK_SPHERE_H
