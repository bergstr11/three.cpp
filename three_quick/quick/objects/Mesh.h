//
// Created by byter on 1/14/18.
//

#ifndef THREEQUICK_MESH_H
#define THREEQUICK_MESH_H

#include <QObject>
#include <objects/Mesh.h>
#include <quick/materials/MeshPhongMaterial.h>
#include <quick/materials/MeshLambertMaterial.h>
#include <quick/materials/MeshBasicMaterial.h>

namespace three {
namespace quick {

using namespace std;

class Mesh : public QObject
{
Q_OBJECT
  Q_PROPERTY(Material * material READ material NOTIFY materialChanged)

  Material *_material;

  three::Mesh::Ptr _mesh;

public:
  Mesh(three::Mesh::Ptr mesh, QObject *parent = nullptr)
     : QObject(parent), _mesh(mesh)
  {
    three::MeshPhongMaterial::Ptr meshPhong = dynamic_pointer_cast<three::MeshPhongMaterial>(mesh->material());
    if(meshPhong) {
      _material = new MeshPhongMaterial(meshPhong, this);
      return;
    }
    three::MeshLambertMaterial::Ptr meshLambert = dynamic_pointer_cast<three::MeshLambertMaterial>(mesh->material());
    if(meshLambert) {
      _material = new MeshLambertMaterial(meshLambert, this);
      return;
    }
    three::MeshBasicMaterial::Ptr meshBasic = dynamic_pointer_cast<three::MeshBasicMaterial>(mesh->material());
    if(meshBasic) {
      _material = new MeshBasicMaterial(meshBasic, this);
      return;
    }
    //three::MeshToonMaterial::Ptr meshToon = dynamic_pointer_cast<three::MeshToonMaterial>(mesh);
    //_material(new Material(mesh->material()), this)
  }

  Material *material() const {return _material;}

signals:
  void materialChanged();
};

}
}


#endif //THREEQUICK_MESH_H
