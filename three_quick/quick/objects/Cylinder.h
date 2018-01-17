//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_QUICK_CYLINDER_H
#define THREEPP_QUICK_CYLINDER_H

#include "quick/scene/Scene.h"
#include <geometry/Cylinder.h>
#include <material/MeshBasicMaterial.h>
#include <material/MeshLambertMaterial.h>
#include <objects/Mesh.h>

namespace three {
namespace quick {

class Cylinder : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(qreal radiusTop READ radiusTop WRITE setRadiusTop NOTIFY radiusTopChanged)
  Q_PROPERTY(qreal radiusBottom READ radiusBottom WRITE setRadiusBottom NOTIFY radiusBottomChanged)
  Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
  Q_PROPERTY(qreal thetaStart READ thetaStart WRITE setThetaStart NOTIFY thetaStartChanged)
  Q_PROPERTY(qreal thetaLength READ thetaLength WRITE setThetaLength NOTIFY thetaLengthChanged)
  Q_PROPERTY(unsigned radialSegments READ radialSegments WRITE setRadialSegments NOTIFY radialSegmentsChanged)
  Q_PROPERTY(unsigned heightSegments READ heightSegments WRITE setHeightSegments NOTIFY heightSegmentsChanged)
  Q_PROPERTY(bool openEnded READ openEnded WRITE setOpenEnded NOTIFY openEndedChanged)

  geometry::CylinderParams _params;

  MeshCreatorG<geometry::Cylinder> _creator {"cylinder"};

protected:
  three::Object3D::Ptr _create(Scene *scene) override
  {
    _creator.set(geometry::Cylinder::make(_params));
    material()->identify(_creator);

    three::Mesh::Ptr mesh = _creator.mesh;

    return mesh;
  }

  void updateMaterial() override {
    material()->identify(_creator);
  }

public:
  Cylinder(QObject *parent = nullptr) : ThreeQObject(parent) {}

  qreal radiusTop() {return _params.radiusTop;}
  void setRadiusTop(qreal radius) {
    if(_params.radiusTop != radius) {
      _params.radiusTop = radius;
      emit radiusTopChanged();
    }
  }
  qreal radiusBottom() {return _params.radiusBottom;}
  void setRadiusBottom(qreal radius) {
    if(_params.radiusBottom != radius) {
      _params.radiusBottom = radius;
      emit radiusBottomChanged();
    }
  }

  unsigned radialSegments() const {return _params.radialSegments;}
  void setRadialSegments(unsigned segments) {
    if(_params.radialSegments != segments) {
      _params.radialSegments = segments;
      emit radialSegmentsChanged();
    }
  }
  unsigned heightSegments() const {return _params.heightSegments;}
  void setHeightSegments(unsigned heightSegments) {
    if(_params.heightSegments != heightSegments) {
      _params.heightSegments = heightSegments;
      emit heightSegmentsChanged();
    }
  }

  qreal thetaStart() const {return _params.thetaStart;}
  void setThetaStart(qreal thetaStart) {
    if(_params.thetaStart != thetaStart) {
      _params.thetaStart = thetaStart;
      emit thetaStartChanged();
    }
  }
  qreal thetaLength() const {return _params.thetaLength;}
  void setThetaLength(qreal thetaLength) {
    if(_params.thetaLength != thetaLength) {
      _params.thetaLength = thetaLength;
      emit thetaLengthChanged();
    }
  }

  unsigned height() const {return _params.height;}
  void setHeight(unsigned height) {
    if(_params.height != height) {
      _params.height = height;
      emit heightChanged();
    }
  }

  bool openEnded() const {return _params.openEnded;}
  void setOpenEnded(qreal openEnded) {
    if(_params.openEnded != openEnded) {
      _params.openEnded = openEnded;
      emit openEndedChanged();
    }
  }

signals:
  void radialSegmentsChanged();
  void heightSegmentsChanged();
  void heightChanged();
  void radiusTopChanged();
  void radiusBottomChanged();
  void thetaStartChanged();
  void thetaLengthChanged();
  void openEndedChanged();
};

}
}

#endif //THREEPP_QUICK_CYLINDER_H
