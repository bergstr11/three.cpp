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

static const char *root = "$root";

class Loader : public QObject, public three::ResourceLoader
{
Q_OBJECT

  QFileInfo file;
  QDir dir;

public:
  Loader(QUrl url) : file(url.toLocalFile()), dir(file.absoluteDir()) {}

  void load() {
    loader::Assimp assimp;
    assimp.load(root, *this);

    emit completed();
    deleteLater();
  }

  bool exists(const char *path) override
  {
    if(!strcmp(path, root)) {
      return file.exists();
    }
    else {
      return dir.exists(QString(path));
    }
  }

  std::istream &&istream(const char *path, ios_base::openmode openmode) override
  {
    if(!strcmp(path, root)) {
      return ifstream(file.absoluteFilePath().toStdString(), openmode);
    }
    else {
      return ifstream(dir.absoluteFilePath(QString(path)).toStdString());
    }
  }

signals:
  void completed();
};

void Model::setFile(const QUrl &file)
{
  if (_file != file) {
    _file = file;
    emit fileChanged();

    QThread *thread = new QThread();
    Loader *loader = new Loader(file);
    loader->moveToThread(thread);
    QObject::connect(loader, &Loader::completed, this, &Model::modelLoaded);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
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

void Model::addTo(ObjectRootContainer *container)
{
}

}
}

#include "Model.moc"