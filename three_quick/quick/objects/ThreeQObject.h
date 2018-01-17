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

class Scene;
class ObjectContainer;

class ThreeQObject : public QObject
{
Q_OBJECT
  Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(Material * material READ material WRITE setMaterial NOTIFY materialChanged)
  Q_PROPERTY(bool castShadow READ castShadow WRITE setCastShadow NOTIFY castShadowChanged)
  Q_PROPERTY(bool receiveShadow READ receiveShadow WRITE setReceiveShadow NOTIFY receiveShadowChanged)
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
  Q_PROPERTY(bool matrixAutoUpdate READ matrixAutoUpdate WRITE setMatrixAutoUpdate NOTIFY matrixAutoUpdateChanged)

protected:
  QVector3D _position {0.0, 0.0, 0.0};
  QVector3D _rotation {0.0, 0.0, 0.0};

  bool _castShadow = false, _receiveShadow = false, _visible = false, _matrixAutoUpdate = true;

  Material *_material = nullptr;

  three::Object3D::Ptr _object;

  virtual three::Object3D::Ptr _create(Scene *scene) = 0;
  virtual void _post_create(Scene *scene) {}

  ThreeQObject(QObject *parent = nullptr) : QObject(parent) {}

  virtual void updateMaterial() {};

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
      if(_object) updateMaterial();
      emit materialChanged();
    }
  }

  bool matrixAutoUpdate() const {return _matrixAutoUpdate;}

  void setMatrixAutoUpdate(bool matrixAutoUpdate) {
    if(_matrixAutoUpdate != matrixAutoUpdate) {
      _matrixAutoUpdate = matrixAutoUpdate;
      emit matrixAutoUpdateChanged();
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

  bool visible() const {return _visible;}

  void setVisible(bool visible) {
    if(_visible != visible) {
      _visible = visible;

      if(_object) _object->visible() = _visible;
      emit visibleChanged();
    }
  }

  three::Object3D::Ptr object() const {return _object;}

  three::Object3D::Ptr create(Scene *scene)
  {
    _object = _create(scene);
    if(_object) {
      _object->rotation().setX(_rotation.x());
      _object->position().set(_position.x(), _position.y(), _position.z());

      _object->castShadow = _castShadow;
      _object->receiveShadow = _receiveShadow;
      _object->matrixAutoUpdate = _matrixAutoUpdate;
    }
    _post_create(scene);
    return _object;
  }

signals:
  void positionChanged();
  void rotationChanged();
  void materialChanged();
  void castShadowChanged();
  void receiveShadowChanged();
  void visibleChanged();
  void matrixAutoUpdateChanged();
};

}
}


#endif //THREEPP_THREEDOBJECT_H
