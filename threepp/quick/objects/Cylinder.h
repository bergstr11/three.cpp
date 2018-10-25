//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_QUICK_CYLINDER_H
#define THREEPPQ_QUICK_CYLINDER_H

#include <threepp/quick/scene/Scene.h>
#include <threepp/geometry/Cylinder.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/objects/Mesh.h>

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

  float _radiusTop = 20;
  float _radiusBottom = 20;
  float _height = 100;
  unsigned _heightSegments = 1;
  unsigned _radialSegments = 8;
  bool _openEnded = false;
  float _thetaStart = 0;
  float _thetaLength = (float)M_PI * 2;

  DynamicMesh::Ptr _mesh;

protected:
  three::Object3D::Ptr _create() override
  {
    _mesh = DynamicMesh::make(geometry::Cylinder::make(_radiusTop, _radiusBottom, _height, _heightSegments,
                                                       _radialSegments, _openEnded, _thetaStart, _thetaLength));
    _mesh->setMaterial(material()->getMaterial());

    return _mesh;
  }

  void updateMaterial() override {
    _mesh->setMaterial(material()->getMaterial());
  }

public:
  Cylinder(QObject *parent = nullptr) : ThreeQObject(parent) {}

  qreal radiusTop() {return _radiusTop;}
  void setRadiusTop(qreal radius) {
    if(_radiusTop != radius) {
      _radiusTop = radius;
      emit radiusTopChanged();
    }
  }
  qreal radiusBottom() {return _radiusBottom;}
  void setRadiusBottom(qreal radius) {
    if(_radiusBottom != radius) {
      _radiusBottom = radius;
      emit radiusBottomChanged();
    }
  }

  unsigned radialSegments() const {return _radialSegments;}
  void setRadialSegments(unsigned segments) {
    if(_radialSegments != segments) {
      _radialSegments = segments;
      emit radialSegmentsChanged();
    }
  }
  unsigned heightSegments() const {return _heightSegments;}
  void setHeightSegments(unsigned heightSegments) {
    if(_heightSegments != heightSegments) {
      _heightSegments = heightSegments;
      emit heightSegmentsChanged();
    }
  }

  qreal thetaStart() const {return _thetaStart;}
  void setThetaStart(qreal thetaStart) {
    if(_thetaStart != thetaStart) {
      _thetaStart = thetaStart;
      emit thetaStartChanged();
    }
  }
  qreal thetaLength() const {return _thetaLength;}
  void setThetaLength(qreal thetaLength) {
    if(_thetaLength != thetaLength) {
      _thetaLength = thetaLength;
      emit thetaLengthChanged();
    }
  }

  unsigned height() const {return _height;}
  void setHeight(unsigned height) {
    if(_height != height) {
      _height = height;
      emit heightChanged();
    }
  }

  bool openEnded() const {return _openEnded;}
  void setOpenEnded(qreal openEnded) {
    if(_openEnded != openEnded) {
      _openEnded = openEnded;
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

#endif //THREEPPQ_QUICK_CYLINDER_H
