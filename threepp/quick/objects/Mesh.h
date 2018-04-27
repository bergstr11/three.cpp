//
// Created by byter on 1/14/18.
//

#ifndef THREEQUICK_MESH_H
#define THREEQUICK_MESH_H

#include <QObject>
#include <threepp/objects/Mesh.h>
#include <threepp/quick/materials/MeshPhongMaterial.h>
#include <threepp/quick/materials/MeshLambertMaterial.h>
#include <threepp/quick/materials/MeshBasicMaterial.h>
#include <threepp/quick/materials/ShaderMaterial.h>

namespace three {
namespace quick {

using namespace std;

class Mesh : public ThreeQObject
{
Q_OBJECT

  three::Mesh::Ptr _mesh;

protected:
  Object3D::Ptr _create() override
  {
    return nullptr; //Mesh is not QML-creatable
  }

  Mesh(three::Mesh::Ptr mesh, Material *material, QObject *parent = nullptr)
     : ThreeQObject(mesh, material, parent), _mesh(mesh) {}

public:
  static Mesh *create(three::Mesh::Ptr mesh, QObject *parent = nullptr)
  {
    three::MeshPhongMaterial::Ptr meshPhong = dynamic_pointer_cast<three::MeshPhongMaterial>(mesh->material());
    if(meshPhong) {
      return new Mesh(mesh, new MeshPhongMaterial(meshPhong, parent), parent);
    }
    three::MeshLambertMaterial::Ptr meshLambert = dynamic_pointer_cast<three::MeshLambertMaterial>(mesh->material());
    if(meshLambert) {
      return new Mesh(mesh, new MeshLambertMaterial(meshLambert, parent), parent);
    }
    three::MeshBasicMaterial::Ptr meshBasic = dynamic_pointer_cast<three::MeshBasicMaterial>(mesh->material());
    if(meshBasic) {
      return new Mesh(mesh, new MeshBasicMaterial(meshBasic, parent), parent);
    }
    three::ShaderMaterial::Ptr shader = dynamic_pointer_cast<three::ShaderMaterial>(mesh);
    if(shader) {
      return new Mesh(mesh, new ShaderMaterial(shader, parent), parent);
    }
    return nullptr;
  }
};

}
}


#endif //THREEQUICK_MESH_H
