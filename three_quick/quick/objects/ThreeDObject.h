//
// Created by byter on 12/12/17.
//

#ifndef THREEPP_THREEDOBJECT_H
#define THREEPP_THREEDOBJECT_H

#include <QObject>
#include <QVector3D>
#include <QVector4D>
#include <scene/Scene.h>
#include <quick/materials/Material.h>

namespace three {
namespace quick {

class ThreeDObject : public QObject
{
Q_OBJECT
  Q_PROPERTY(QVector4D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(Material * material READ material WRITE setMaterial NOTIFY materialChanged)

protected:
  QVector3D _position;
  QVector4D _rotation;

  Material *_material = nullptr;

  three::Object3D::Ptr _object;

public:
  QVector3D position() {return _position;}
  QVector4D rotation() {return _rotation;}

  void setPosition(const QVector3D &position) {
    if(position != _position) {
      _position = position;
      emit positionChanged();
    }
  }

  void setRotation(const QVector4D &rotation) {
    if(rotation != _rotation) {
      _rotation = rotation;
      emit rotationChanged();
    }
  }

  Material *material() const {return _material;}

  void setMaterial(Material *material) {
    if(_material != material) {
      _material = material;
      emit materialChanged();
    }
  }

  three::Object3D::Ptr object() const {return _object;}

  virtual void addTo(three::Scene::Ptr scene) = 0;

signals:
  void positionChanged();
  void rotationChanged();
  void materialChanged();
};

}
}


#endif //THREEPP_THREEDOBJECT_H
