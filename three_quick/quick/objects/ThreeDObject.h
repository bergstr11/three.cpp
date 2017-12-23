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

class ThreeDScene;

class ThreeDObject : public QObject
{
Q_OBJECT
  Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(Material * material READ material WRITE setMaterial NOTIFY materialChanged)
  Q_PROPERTY(bool castShadow READ castShadow WRITE setCastShadow NOTIFY castShadowChanged)
  Q_PROPERTY(bool receiveShadow READ receiveShadow WRITE setReceiveShadow NOTIFY receiveShadowChanged)

protected:
  QVector3D _position {0.0, 0.0, 0.0};
  QVector3D _rotation {0.0, 0.0, 0.0};

  bool _castShadow = false, _receiveShadow = false, _helper = false;

  Material *_material = nullptr;

  three::Object3D::Ptr _object;

  virtual three::Object3D::Ptr _create(ThreeDScene *scene) = 0;
  virtual void _post_create(ThreeDScene *scene) {}

public:
  QVector3D position() {return _position;}
  QVector3D rotation() {return _rotation;}

  void setPosition(const QVector3D &position) {
    if(position != _position) {
      _position = position;
      emit positionChanged();
    }
  }

  void setRotation(const QVector3D &rotation) {
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

  bool castShadow() const {return _castShadow;}

  void setCastShadow(bool castShadow) {
    if(_castShadow != castShadow) {
      _castShadow = castShadow;
      emit castShadowChanged();
    }
  }

  bool receiveShadow() const {return _receiveShadow;}

  void setReceiveShadow(bool receiveShadow) {
    if(_receiveShadow != receiveShadow) {
      _receiveShadow = receiveShadow;
      emit receiveShadowChanged();
    }
  }

  three::Object3D::Ptr object() const {return _object;}

  three::Object3D::Ptr create(ThreeDScene *scene)
  {
    three::Object3D::Ptr obj = _create(scene);
    if(obj) {
      obj->rotation().setX(_rotation.x());
      obj->position().set(_position.x(), _position.y(), _position.z());

      obj->castShadow = _castShadow;
      obj->receiveShadow = _receiveShadow;
    }
    _post_create(scene);
    return obj;
  }

signals:
  void positionChanged();
  void rotationChanged();
  void materialChanged();
  void castShadowChanged();
  void receiveShadowChanged();
};

}
}


#endif //THREEPP_THREEDOBJECT_H
