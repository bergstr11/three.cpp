//
// Created by byter on 1/12/18.
//

#include "Model.h"

#include <threepp/loader/Assimp.h>
#include <threepp/quick/qutil/FileSystemLoader.h>
#include <threepp/quick/qutil/QtResourceLoader.h>

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

    if(_container) loadFile(file);
  }
}

void Model::setName(const QString &name) {
  if (_name != name) {
    _name = name;
    emit nameChanged();
  }
}

three::Scene::Ptr Model::scene() {
  return _assimp ? _assimp->scene() : nullptr;
}

void Model::addTo(ObjectRootContainer *container)
{
  _container = container;

  if(!(_file.isNull() || _file.isEmpty())) loadFile(_file);
}

}
}