//
// Created by byter on 1/29/18.
//

#ifndef THREEPP_RAYCASTER_H
#define THREEPP_RAYCASTER_H

#include <QVariantList>
#include <QVector2D>
#include <core/Raycaster.h>
#include <quick/cameras/Camera.h>
#include "../ThreeQObjectRoot.h"

namespace three {
namespace quick {

class Intersect : public QObject
{
Q_OBJECT
  Q_PROPERTY(float distance READ distance CONSTANT)
  Q_PROPERTY(QVector3D point MEMBER point CONSTANT)
  Q_PROPERTY(QVector2D uv MEMBER uv CONSTANT)
  Q_PROPERTY(ThreeQObject * object MEMBER object CONSTANT)
  Q_PROPERTY(QVector3D faceNormal MEMBER faceNormal CONSTANT)

public:
  three::Intersection intersection;
  QVector3D point;
  QVector2D uv;
  QVector3D faceNormal;
  ThreeQObject * object;

  float distance() const {return intersection.distance;}

  Intersect(ThreeQObject * object, const Intersection &intersection)
     : QObject(object), object(object), intersection(intersection)
  {
    point.setX(intersection.point.x());
    point.setY(intersection.point.y());
    point.setZ(intersection.point.z());

    uv.setX(intersection.uv.x());
    uv.setY(intersection.uv.y());

    faceNormal.setX(intersection.face.normal.x());
    faceNormal.setY(intersection.face.normal.y());
    faceNormal.setZ(intersection.face.normal.z());
  }
};

class RayCaster : public QObject
{
Q_OBJECT
  three::Raycaster raycaster;
  Camera *_camera;

public:
  Q_INVOKABLE QVariantList intersectObjects(const QVariantList &objects);
  Q_INVOKABLE void set(const QVector2D &position);

  RayCaster(Camera *camera, QObject *parent=nullptr) : QObject(parent), _camera(camera) {}
};

}
}

#endif //THREEPP_RAYCASTER_H
