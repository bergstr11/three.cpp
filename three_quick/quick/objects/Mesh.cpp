//
// Created by byter on 1/12/18.
//

#include "Mesh.h"

namespace three {
namespace quick {

void Mesh::setModel(Model *model) {
  if(_model != model) {
    if(_modelConnection) QObject::disconnect(_modelConnection);
    _model = model;
    if(_model)
      _modelConnection = QObject::connect(_model, &Model::modelLoaded, this, &Mesh::updateMesh);
    emit modelChanged();
  }
}

void Mesh::setPath(const QString &path) {
  if(_path != path) {
    _path = path;
    emit pathChanged();
  }
}

void Mesh::updateMesh()
{
  if(_object)
    _scene->scene()->remove(_object);

  _object = _model->scene()->getChildByName(_path.toStdString());

  if(_object)
    _scene->scene()->add(_object);
}

}
}