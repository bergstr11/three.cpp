//
// Created by byter on 12/12/17.
//

#ifndef THREEPPQ_THREEDOBJECT_H
#define THREEPPQ_THREEDOBJECT_H

#include <QObject>
#include <QVector3D>
#include <QVector4D>
#include <threepp/scene/Scene.h>
#include <threepp/quick/materials/Material.h>

namespace three {
namespace quick {

class Scene;
class ObjectContainer;

class ThreeQObject : public QObject
{
Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
  Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
  Q_PROPERTY(Material * material READ material WRITE setMaterial NOTIFY materialChanged)
  Q_PROPERTY(bool castShadow READ castShadow WRITE setCastShadow NOTIFY castShadowChanged)
  Q_PROPERTY(bool receiveShadow READ receiveShadow WRITE setReceiveShadow NOTIFY receiveShadowChanged)
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
  Q_PROPERTY(bool matrixAutoUpdate READ matrixAutoUpdate WRITE setMatrixAutoUpdate NOTIFY matrixAutoUpdateChanged)

protected:
  QString _name;

  QVector3D _position;

  QVector3D _rotation;

  float _scale = 1.0f;

  bool _castShadow = false, _receiveShadow = false, _visible = false, _matrixAutoUpdate = true;

  Material *_material = nullptr;

  three::Object3D::Ptr _object;

  virtual three::Object3D::Ptr _create(Scene *scene) = 0;
  virtual void _post_create(Scene *scene) {}

  ThreeQObject(QObject *parent = nullptr) : QObject(parent) {}
  ThreeQObject(three::Object3D::Ptr object, QObject *parent = nullptr)
     : QObject(parent), _object(object) {}

  virtual void updateMaterial() {}

public:
  QVector3D position() const {return _position;}
  QVector3D rotation() const {return _rotation;}
  float scale() const {return _scale;}

  void setObject(three::Object3D::Ptr object);

  void setPosition(const QVector3D &position, bool propagate=true) {
    if(position != _position) {
      _position = position;

      if(propagate)
        if(_object) _object->position().set(_position.x(), _position.y(), _position.z());

      emit positionChanged();
    }
  }

  void setRotation(const QVector3D &rotation, bool propagate=true) {
    if(_rotation != rotation) {
      _rotation = rotation;

      if(propagate)
        if(_object) _object->rotation().set(_rotation.x(), _rotation.y(), _rotation.z());

      emit rotationChanged();
    }
  }

  void setScale(float scale) {
    if(_scale != scale) {
      _scale = scale;
      emit scaleChanged();
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

  void setMatrixAutoUpdate(bool matrixAutoUpdate, bool propagate=true) {
    if(_matrixAutoUpdate != matrixAutoUpdate) {
      _matrixAutoUpdate = matrixAutoUpdate;
      if(propagate && _object) _object->matrixAutoUpdate = _matrixAutoUpdate;
      emit matrixAutoUpdateChanged();
    }
  }

  bool castShadow() const {return _castShadow;}

  void setCastShadow(bool castShadow, bool propagate=true) {
    if(_castShadow != castShadow) {
      _castShadow = castShadow;
      if(propagate && _object) _object->castShadow = _castShadow;
      emit castShadowChanged();
    }
  }

  bool receiveShadow() const {return _receiveShadow;}

  void setReceiveShadow(bool receiveShadow, bool propagate=true) {
    if(_receiveShadow != receiveShadow) {
      _receiveShadow = receiveShadow;
      if(propagate && _object) _object->receiveShadow = _castShadow;
      emit receiveShadowChanged();
    }
  }

  bool visible() const {return _visible;}

  void setVisible(bool visible, bool propagate=true) {
    if(_visible != visible) {
      _visible = visible;

      if(propagate && _object) _object->visible() = _visible;
      emit visibleChanged();
    }
  }

  const QString &name() const {return _name;}

  void setName(const QString &name, bool propagate=true)
  {
    if(_name != name) {
      _name = name;
      if(_object && propagate) _object->setName(_name.toStdString());

      emit nameChanged();
    }
  }

  three::Object3D::Ptr object() const {return _object;}

  three::Object3D::Ptr create(Scene *scene);

  Q_INVOKABLE void rotateX(float angle);
  Q_INVOKABLE void rotateY(float angle);
  Q_INVOKABLE void rotateZ(float angle);
  Q_INVOKABLE void translateZ(float distance);

signals:
  void positionChanged();
  void rotationChanged();
  void scaleChanged();
  void materialChanged();
  void castShadowChanged();
  void receiveShadowChanged();
  void visibleChanged();
  void matrixAutoUpdateChanged();
  void nameChanged();
};

}
}


#endif //THREEPPQ_THREEDOBJECT_H
