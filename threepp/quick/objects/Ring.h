//
// Created by byter on 2/9/18.
//

#ifndef THREE_PPQ_RING_H
#define THREE_PPQ_RING_H

#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/geometry/Ring.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/objects/Mesh.h>

namespace three {
namespace quick {

class Ring : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(float innerRadius READ innerRadius WRITE setInnerRadius NOTIFY innerRadiusChanged)
  Q_PROPERTY(float outerRadius READ outerRadius WRITE setOuterRadius NOTIFY outerRadiusChanged)
  Q_PROPERTY(float thetaStart READ thetaStart WRITE setThetaStart NOTIFY thetaStartChanged)
  Q_PROPERTY(float thetaLength READ thetaLength WRITE setThetaLength NOTIFY thetaLengthChanged)
  Q_PROPERTY(unsigned thetaSegments READ thetaSegments WRITE setThetaSegments NOTIFY thetaSegmentsChanged)
  Q_PROPERTY(unsigned phiSegments READ phiSegments WRITE setPhiSegments NOTIFY phiSegmentsChanged)

  geometry::RingParams _params;

  DynamicMesh::Ptr _mesh;

protected:
  three::Object3D::Ptr _create() override
  {
    _mesh = DynamicMesh::make(geometry::Ring::make(_params));
    _mesh->setMaterial(material()->getMaterial());

    return _mesh;
  }

  void updateMaterial() override {
    _mesh->setMaterial(material()->getMaterial());
  }

public:
  Ring(QObject *parent = nullptr) : ThreeQObject(parent) {}

  float innerRadius() const {return _params.innerRadius;}
  float outerRadius() const {return _params.outerRadius;}
  unsigned thetaSegments() const {return _params.thetaSegments;}
  unsigned phiSegments() const {return _params.phiSegments;}
  float thetaStart() const {return _params.thetaStart;}
  float thetaLength() const {return _params.thetaLength;};

  void setInnerRadius(float innerRadius)
  {
    if(_params.innerRadius != innerRadius) {
      _params.innerRadius = innerRadius;
      emit innerRadiusChanged();
    }
  }

  void setOuterRadius(float outerRadius)
  {
    if(_params.outerRadius != outerRadius) {
      _params.outerRadius = outerRadius;
      emit outerRadiusChanged();
    }
  }

  void setThetaSegments(unsigned thetaSegments)
  {
    if(_params.thetaSegments != thetaSegments) {
      _params.thetaSegments = thetaSegments;
      emit thetaSegmentsChanged();
    }
  }

  void setPhiSegments(unsigned phiSegments)
  {
    if(_params.phiSegments != phiSegments) {
      _params.phiSegments = phiSegments;
      emit phiSegmentsChanged();
    }
  }

  void setThetaStart(float thetaStart)
  {
    if(_params.thetaStart != thetaStart) {
      _params.thetaStart = thetaStart;
      emit thetaStartChanged();
    }
  }

  void setThetaLength(float thetaLength)
  {
    if(_params.thetaLength != thetaLength) {
      _params.thetaLength = thetaLength;
      emit thetaLengthChanged();
    }
  }

signals:
  void thetaSegmentsChanged();
  void phiSegmentsChanged();
  void thetaStartChanged();
  void thetaLengthChanged();
  void innerRadiusChanged();
  void outerRadiusChanged();
};

};
}

#endif //THREE_PPQ_RING_H
