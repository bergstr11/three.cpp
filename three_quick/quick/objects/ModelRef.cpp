//
// Created by byter on 1/12/18.
//

#include "ModelRef.h"

namespace three {
namespace quick {

void ModelRef::setModel(Model *model) {
  if(_model != model) {
    if(_loadedConnection) QObject::disconnect(_loadedConnection);
    if(_fileConnection) QObject::disconnect(_fileConnection);
    _model = model;
    if(_model) {
      _fileConnection = QObject::connect(_model, &Model::fileChanged, this, &ModelRef::cleanupMesh);
      _loadedConnection = QObject::connect(_model, &Model::modelLoaded, this, &ModelRef::updateMesh);
    }
    emit modelChanged();
  }
}

void ModelRef::cleanupMesh()
{
  for(const auto &obj : _objects) {
    _scene->scene()->remove(obj);
  }
  _objects.clear();
}

void ModelRef::updateMesh()
{
  switch(_type) {
    case Mesh: {
      _objects = _model->scene()->children();
      for(const auto &obj : _objects) {
        _scene->scene()->add(obj);
      }
      emit _scene->sceneChanged();
      break;
    }
    case Texture:
      break;
    case Light:
      break;
    case Camera:
      break;
  }
}

}
}
