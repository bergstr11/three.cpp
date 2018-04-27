//
// Created by byter on 2/9/18.
//

#ifndef THREE_PPQ_TORUS_H
#define THREE_PPQ_TORUS_H

#include <threepp/quick/scene/Scene.h>
#include <threepp/geometry/Torus.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/objects/Mesh.h>

namespace three {
namespace quick {

class Torus : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(float tube READ tube WRITE setTube NOTIFY tubeChanged)
  Q_PROPERTY(float arc READ arc WRITE setArc NOTIFY arcChanged)
  Q_PROPERTY(unsigned radialSegments READ radialSegments WRITE setRadialSegments NOTIFY radialSegmentsChanged)
  Q_PROPERTY(unsigned tubularSegments READ tubularSegments WRITE setTubularSegments NOTIFY tubularSegmentsChanged)

  geometry::TorusParams _params;

  MeshCreatorG<geometry::Torus> _creator {"torus"};

protected:
  three::Object3D::Ptr _create() override
  {
    _creator.set(geometry::Torus::make(_params));
    material()->identify(_creator);

    return _creator.mesh;
  }

  void updateMaterial() override {
    material()->identify(_creator);
  }

public:
  Torus(QObject *parent = nullptr) : ThreeQObject(parent) {}

  float radius() const {return _params.radius;}
  float tube() const {return _params.tube;}
  float arc() const {return _params.arc;}
  unsigned radialSegments() const {return _params.radialSegments;}
  unsigned tubularSegments() const {return _params.tubularSegments;}

  void setRadius(float radius)
  {
    if(_params.radius != radius) {
      _params.radius = radius;
      emit radiusChanged();
    }
  }

  void setTube(float tube)
  {
    if(_params.tube != tube) {
      _params.tube = tube;
      emit tubeChanged();
    }
  }

  void setArc(float arc)
  {
    if(_params.arc != arc) {
      _params.arc = arc;
      emit arcChanged();
    }
  }

  void setRadialSegments(unsigned radialSegments)
  {
    if(_params.radialSegments != radialSegments) {
      _params.radialSegments = radialSegments;
      emit radialSegmentsChanged();
    }
  }

  void setTubularSegments(unsigned tubularSegments)
  {
    if(_params.tubularSegments != tubularSegments) {
      _params.tubularSegments = tubularSegments;
      emit tubularSegmentsChanged();
    }
  }

signals:
  void radiusChanged();
  void tubeChanged();
  void arcChanged();
  void radialSegmentsChanged();
  void tubularSegmentsChanged();
};

};
}

#endif //THREE_PPQ_TORUS_H
