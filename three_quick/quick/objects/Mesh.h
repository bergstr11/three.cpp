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

class Mesh : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(Model *model READ model WRITE setModel NOTIFY modelChanged)
  Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

  Model *_model = nullptr;
  QString _path;

  QMetaObject::Connection _modelConnection;

  Scene *_scene = nullptr;

  Object3D::Ptr _object;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _scene = scene;
    return nullptr;
  }

public:
  Model *model() const {return _model;}

  void setModel(Model *model);

  const QString &path() const {return _path;}

  void setPath(const QString &path);

signals:
  void modelChanged();
  void pathChanged();

private slots:
  void updateMesh();
};

}
}

#endif //THREEPP_MESH_H
