//
// Created by byter on 1/12/18.
//

#include "Model.h"

#include <QQuaternion>
#include <threepp/quick/loader/FileSystemLoader.h>
#include <threepp/quick/loader/QtResourceLoader.h>
#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/quick/elements/RayCaster.h>
#include <threepp/quick/objects/Mesh.h>
#include <threepp/quick/objects/Node.h>

namespace three {
namespace quick {

using namespace std;

bool MaterialHandler::hasName(const std::string &name)
{
  const auto &names = _materialNames.splitRef(',', QString::SkipEmptyParts);
  for(const auto &nm : names) {
    if(nm.trimmed() == name.c_str()) return true;
  }
  return false;
}

void MaterialHandler::setAssimpHandlerCreateAs()
{
  const auto &names = _materialNames.split(',', QString::SkipEmptyParts);
  for(const auto &name : names) {
    if(MeshPhongMaterial *mat = _createAs->typer) {
      _assimpHandler->createAs<three::MeshPhongMaterial>(name.trimmed().toStdString());
    }
    else if(MeshLambertMaterial *mat = _createAs->typer) {
      _assimpHandler->createAs<three::MeshLambertMaterial>(name.trimmed().toStdString());
    }
    else if(MeshBasicMaterial *mat = _createAs->typer) {
      _assimpHandler->createAs<three::MeshBasicMaterial>(name.trimmed().toStdString());
    }
    else if(MeshPhysicalMaterial *mat = _createAs->typer) {
      _assimpHandler->createAs<three::MeshPhysicalMaterial>(name.trimmed().toStdString());
    }
    else if(MeshStandardMaterial *mat = _createAs->typer) {
      _assimpHandler->createAs<three::MeshStandardMaterial>(name.trimmed().toStdString());
    }
  }
}

void MaterialHandler::setLoadedMaterial(three::Material::Ptr material)
{
  _createAs->setAndConfigure(material);
  QVariant var;
  var.setValue(_createAs);
  emit loaded(var);
}

void Model::setReplacements(const QVariantMap &replacements)
{
  if(_replacements != replacements) {
    _replacements = replacements;
    emit replacementsChanged();
  }
}

void Options::setAssimp(shared_ptr<loader::Assimp> assimp)
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

  _assimp = _materialHandler.handlers().isEmpty() ?
            make_shared<loader::Assimp>() : make_shared<loader::Assimp>(&_materialHandler);
  _options.setAssimp(_assimp);

  //first check if this is a file: url and convert
  QUrl fileUrl(file);
  //file may be either a file url, a resource url or a file system path
  QFileInfo info(fileUrl.isLocalFile() ? fileUrl.toLocalFile() : file);

  if(info.isNativePath()) {
    info.makeAbsolute();
    FileSystemLoader *loader = new FileSystemLoader(*_assimp, info, replacements);
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

void setLoadedMaterial(const QList<quick::MaterialHandler *> &handlers, const string &name, three::Material::Ptr mp)
{
  for(const auto &handler : handlers) {
    if(handler->hasName(name)) {

      handler->setLoadedMaterial(mp);
    }
  }
}

void ModelMaterialHandler::handle(const string &name, three::MeshPhongMaterial &material, three::Material::Ptr mp) const
{
  setLoadedMaterial(_handlers, name, mp);
}
void ModelMaterialHandler::handle(const string &name, three::MeshToonMaterial &material, three::Material::Ptr mp) const
{
  setLoadedMaterial(_handlers, name, mp);
}
void ModelMaterialHandler::handle(const string &name, three::MeshLambertMaterial &material, three::Material::Ptr mp) const
{
  setLoadedMaterial(_handlers, name, mp);
}
void ModelMaterialHandler::handle(const string &name, three::MeshStandardMaterial &material, three::Material::Ptr mp) const
{
  setLoadedMaterial(_handlers, name, mp);
}
void ModelMaterialHandler::handle(const string &name, three::MeshPhysicalMaterial &material, three::Material::Ptr mp) const
{
  setLoadedMaterial(_handlers, name, mp);
}
void ModelMaterialHandler::handle(const string &name, three::MeshBasicMaterial &material, three::Material::Ptr mp) const
{
  setLoadedMaterial(_handlers, name, mp);
}

void Model::append_handler(QQmlListProperty<quick::MaterialHandler> *list, quick::MaterialHandler *obj)
{
  Model *model = qobject_cast<Model *>(list->object);
  if (model) {
    obj->setAssimpHandler(&model->_materialHandler);
    model->_materialHandler.handlers().append(obj);
  }
}
int Model::count_handlers(QQmlListProperty<quick::MaterialHandler> *list)
{
  Model *model = qobject_cast<Model *>(list->object);
  return model ? model->_materialHandler.handlers().size() : 0;
}
MaterialHandler *Model::handler_at(QQmlListProperty<quick::MaterialHandler> *list, int index)
{
  Model *model = qobject_cast<Model *>(list->object);
  return model ? model->_materialHandler.handlers().at(index) : nullptr;
}
void Model::clear_handlers(QQmlListProperty<quick::MaterialHandler> *list)
{
  Model *model = qobject_cast<Model *>(list->object);
  if(model) model->_materialHandler.handlers().clear();
}

QQmlListProperty<MaterialHandler> Model::handlers()
{
  return QQmlListProperty<quick::MaterialHandler>(this, nullptr,
                                                  &Model::append_handler,
                                                  &Model::count_handlers,
                                                  &Model::handler_at,
                                                  &Model::clear_handlers);
}

}
}
