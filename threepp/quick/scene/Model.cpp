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

void Options::setAssimp(std::shared_ptr<loader::Assimp> assimp)
{
  _assimp = assimp;

  loader::ShadingModel newShading = _preferPhong ?
                                    loader::ShadingModel::Phong : loader::ShadingModel::Gouraud;
  _assimp->substituteShading(loader::ShadingModel::Gouraud, newShading);
}

void Options::setPreferPhong(bool prefer)
{
  if(_preferPhong != prefer) {
    _preferPhong = prefer;

    if(_assimp) {
      loader::ShadingModel newShading = _preferPhong ?
                                        loader::ShadingModel::Phong : loader::ShadingModel::Gouraud;
      _assimp->substituteShading(loader::ShadingModel::Gouraud, newShading);
    }

    emit preferPhongChanged();
  }
}

void Model::loadFile(const QString &file)
{
  unordered_map<string, string> replacements;
  for(const auto &repl : _replacements.keys()) {
    replacements[repl.toStdString()] = _replacements[repl].toString().toStdString();
  }

  _assimp = make_shared<loader::Assimp>();
  _options.setAssimp(_assimp);

  QFileInfo info(file);
  if(info.isNativePath()) {
    FileSystemLoader *loader = new FileSystemLoader(*_assimp, file, replacements);
    QObject::connect(loader, &FileSystemLoader::loaded, this, &Model::modelLoaded);
    loader->start();
  }
  else {
    QtResourceLoader *loader = new QtResourceLoader(*_assimp, file, replacements);
    if(!_resourcePrefix.isEmpty()) loader->setResourcePrefix(_resourcePrefix);
    QObject::connect(loader, &QtResourceLoader::loaded, this, &Model::modelLoaded);
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

void Model::setResourcePrefix(const QString &prefix) {
  if (_resourcePrefix != prefix) {
    _resourcePrefix = prefix;
    emit resourcePrefixChanged();
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

}
}
