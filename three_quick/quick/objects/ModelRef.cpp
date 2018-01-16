//
// Created by byter on 1/12/18.
//

#include <QDebug>
#include "ModelRef.h"
#include <quick/objects/Mesh.h>

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
      if(_selector == "*") {
        _objects = _model->scene()->children();
        for(const auto &obj : _objects) {
          _scene->scene()->add(obj);
        }
      }
      else {
        Object3D::Ptr child = _model->scene();
        three::Mesh::Ptr mesh;
        do {
          child = child->getChildByName(_selector.toStdString());
          mesh = std::dynamic_pointer_cast<three::Mesh>(child);
        }
        while(child && !mesh);

        if(mesh) {
          _objects.clear();
          _objects.push_back(child);

          switch(_type) {
            case Mesh:
              _object = new three::quick::Mesh(mesh, this);
              _scene->scene()->add(mesh);
              emit modelObjectChanged();
              break;
            case Texture:
              break;
            case Light:
              break;
            case Camera:
              break;
          }
        }
        else qDebug() << "child not found or type not matched";
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
