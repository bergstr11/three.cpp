//
// Created by byter on 1/12/18.
//

#include "Model.h"

#include <QQuaternion>
#include <threepp/loader/Assimp.h>
#include <threepp/quick/loader/FileSystemLoader.h>
#include <threepp/quick/loader/QtResourceLoader.h>
#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/quick/elements/RayCaster.h>
#include <threepp/quick/objects/Mesh.h>
#include <threepp/quick/objects/Node.h>

namespace three {
namespace quick {

using namespace std;

void Model::setReplacements(const QVariantMap &replacements)
{
  if(_replacements != replacements) {
    _replacements = replacements;
    emit replacementsChanged();
  }
}

void Model::loadFile(const QString &file)
{
  unordered_map<string, string> replacements;
  for(const auto &repl : _replacements.keys()) {
    replacements[repl.toStdString()] = _replacements[repl].toString().toStdString();
  }

  _assimp = make_shared<loader::Assimp>();

  QFileInfo info(file);
  if(info.isNativePath()) {
    FileSystemLoader *loader = new FileSystemLoader(*_assimp, file, replacements);
    QObject::connect(loader, &FileSystemLoader::loaded, this, &Model::modelLoaded);
    QObject::connect(loader, &QThread::finished, loader, &QObject::deleteLater);
    loader->start();
  }
  else {
    QtResourceLoader *loader = new QtResourceLoader(*_assimp, file, replacements);
    QObject::connect(loader, &QtResourceLoader::loaded, this, &Model::modelLoaded);
    QObject::connect(loader, &QThread::finished, loader, &QObject::deleteLater);
    loader->start();
  }
}

void Model::setFile(const QString &file)
{
  if (_file != file) {
    _file = file;
    emit fileChanged();

    if(_item) loadFile(file);
  }
}

void Model::setName(const QString &name) {
  if (_name != name) {
    _name = name;
    emit nameChanged();
  }
}

three::Scene::Ptr Model::importedScene() {
  return _assimp ? _assimp->scene() : nullptr;
}

void Model::setItem(ThreeDItem *item)
{
  _item = item;
  if(_item && (!(_file.isNull() || _file.isEmpty()))) loadFile(_file);
}

ThreeQObject *Model::createObject(const QByteArray &name, const QVector3D &position,
                                  const QVector3D &modelNormal, const QVector3D &faceNormal,
                                  float scale, const QVariantMap &arguments)
{
  Object3D::Ptr object = importedScene()->getChildByName(name.toStdString());
  if(object) {
    Object3D::Ptr clone(object->cloned());
    ThreeQObject *three = nullptr;

    three::Mesh::Ptr mesh = dynamic_pointer_cast<three::Mesh>(clone);
    if(mesh) {
      three = Mesh::create(mesh, this);
    }
    else {
      three::Node::Ptr node = dynamic_pointer_cast<three::Node>(clone);
      if(node) three = new Node(node, this);
    }
    if(!three) return nullptr;

    three->object()->scale() = scale;

    three->object()->position().set(position.x(), position.y(), position.z());


    enum {X, Y, Z} m_up = modelNormal.x() ? X : (modelNormal.y() ? Y : Z);

    //calculate the rotation matrix for aligning the marker to the mesh surface
    math::Vector3 up(0, 0, 0);
    math::Vector3 normal(faceNormal.x(), faceNormal.y(), faceNormal.z());
    math::Vector3 axis;
    switch(m_up) {
      case X:
        up.x() = 1;
        axis = normal.x() == 1 || normal.x() == -1 ? math::Vector3( 0, 1, 0 ) : cross(up, normal);
        break;
      case Y:
        up.y() = 1;
        axis = normal.y() == 1 || normal.y() == -1 ? math::Vector3( 1, 0, 0 ) : cross(up, normal);
        break;
      case Z:
        up.z() = 1;
        axis = normal.z() == 1 || normal.z() == -1 ? math::Vector3( 1, 0, 0 ) : cross(up, normal);
        break;
    }
    double angle = acos(dot(normal, up));
    //GLC_Matrix4x4 rotation(axis, radians);

    //QVector3D axis = QVector3D::crossProduct(modelNormal, faceNormal).normalized();
    //float angle = acos(QVector3D::dotProduct(modelNormal, faceNormal));

    three->object()->setRotationFromAxisAngle(axis, angle);
    three->object()->updateMatrix();

    for(auto it = arguments.keyBegin(); it != arguments.keyEnd(); it++) {
      three->setProperty(it->toLocal8Bit(), arguments[*it]);
    }
    return three;
  }
  return nullptr;
}

}
}
