//
// Created by byter on 1/12/18.
//

#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <fstream>
#include <loader/Assimp.h>

#include "Model.h"

namespace three {
namespace quick {

using namespace std;

class Resource : public three::Resource
{
  friend class Loader;

  fstream fin;
  QFileInfo info;

  Resource(const QString &path, ios_base::openmode openmode)
     : fin(path.toStdString(), openmode), info(path)
  {
    if(!info.exists() || fin.fail()) throw invalid_argument("cannot read file: "+path.toStdString());
  }

public:
  using Ptr = shared_ptr<Resource>;

  static Ptr make(const QString &path, ios_base::openmode openmode) {
    return Ptr(new Resource(path, openmode));
  }

  size_t size() override
  {
    return info.size();
  }

  istream &in() override
  {
    return fin;
  }
};

class Loader : public QThread, public three::ResourceLoader
{
Q_OBJECT

  loader::Assimp &assimp;
  QFileInfo file;
  QDir dir;

public:
  Loader(loader::Assimp &assimp, QUrl url)
     : assimp(assimp), file(url.toLocalFile()), dir(file.absoluteDir()) {}

  void run() override
  {
    assimp.load(file.fileName().toStdString(), *this);

    emit completed();
    deleteLater();
  }

  bool exists(const char *path) override
  {
    if(file.fileName() == path) {
      return file.exists();
    }
    else {
      return dir.exists(QString(path));
    }
  }

  three::Resource::Ptr get(const char *path, ios_base::openmode openmode) override
  {
    if(file.fileName() == path) {
      return Resource::make(file.absoluteFilePath(), openmode);
    }
    else {
      return Resource::make(dir.absoluteFilePath(QString(path)), openmode);
    }
  }

  string makePath(const string &file) override
  {
    return dir.absoluteFilePath(QString::fromStdString(file)).toStdString();
  }

signals:
  void completed();
};

void Model::setFile(const QUrl &file)
{
  if (_file != file) {
    _file = file;
    emit fileChanged();

    _assimp = make_shared<loader::Assimp>();
    Loader *loader = new Loader(*_assimp, file);
    QObject::connect(loader, &Loader::completed, this, &Model::modelLoaded);
    loader->start();
  }
}

void Model::setName(const QString &name) {
  if (_name != name) {
    _name = name;
    emit nameChanged();
  }
}

void Model::setIsScene(bool isScene) {
  if (_isScene != isScene) {
    _isScene = isScene;
    emit isSceneChanged();
  }
}

three::Scene::Ptr Model::scene() {
  return _assimp ? _assimp->scene() : nullptr;
}

void Model::addTo(ObjectRootContainer *container)
{
}

}
}

#include "Model.moc"
