//
// Created by byter on 1/12/18.
//

#ifndef THREEPP_MESH_H
#define THREEPP_MESH_H

#include <QMetaObject>
#include "quick/scene/Model.h"
#include "quick/scene/Scene.h"
#include "quick/objects/ThreeQObject.h"

namespace three {
namespace quick {

class ModelRef : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(Model *model READ model WRITE setModel NOTIFY modelChanged)
  Q_PROPERTY(QString selector READ selector WRITE setSelector)
  Q_PROPERTY(Type type READ type WRITE setType)

public:
  enum Type {Mesh, Texture, Light, Camera};
  Q_ENUM(Type)

private:
  Model *_model = nullptr;
  QString _selector;
  Type _type;

  QMetaObject::Connection _loadedConnection;
  QMetaObject::Connection _fileConnection;

  Scene *_scene = nullptr;

  std::vector<Object3D::Ptr> _objects;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _scene = scene;
    return nullptr;
  }

public:
  ModelRef(QObject *parent=nullptr) : ThreeQObject(parent) {}

  Model *model() const {return _model;}

  void setModel(Model *model);

  const QString &selector() const {return _selector;}

  void setSelector(const QString &selector) {_selector = selector;}

  Type type() const {return _type;}

  void setType(Type type) {_type = type;}

signals:
  void modelChanged();

private slots:
  void cleanupMesh();
  void updateMesh();
};

}
}

#endif //THREEPP_MESH_H
