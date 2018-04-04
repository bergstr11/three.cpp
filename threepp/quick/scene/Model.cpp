//
// Created by byter on 1/12/18.
//

#include "Model.h"

#include <threepp/loader/Assimp.h>
#include <threepp/quick/loader/FileSystemLoader.h>
#include <threepp/quick/loader/QtResourceLoader.h>
#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/quick/elements/RayCaster.h>
#include <threepp/quick/objects/Mesh.h>

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

ThreeQObject *Model::createObject(QByteArray name, Intersect *intersect, const QVariantMap &arguments)
{
  Object3D::Ptr object = importedScene()->getChildByName(name.toStdString());
  if(object) {
    Object3D::Ptr clone(object->cloned());
    three::Mesh::Ptr mesh = dynamic_pointer_cast<three::Mesh>(clone);
    ThreeQObject *three = mesh ? Mesh::create(mesh, this) : new ThreeQObject(clone, this);

    three->object()->position().set(intersect->point.x(), intersect->point.y(), intersect->point.z());
    three->object()->setRotationFromAxisAngle(
       math::Vector3(intersect->faceNormal.x(), intersect->faceNormal.y(), intersect->faceNormal.z()), 0);
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
