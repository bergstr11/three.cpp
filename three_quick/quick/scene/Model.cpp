//
// Created by byter on 1/12/18.
//

#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <fstream>
#include <loader/Assimp.h>
#include <QDebug>
#include <algorithm>

#include "Model.h"

namespace three {
namespace quick {

using namespace std;

/**
 * represents a resource loaded from a Qt resource
 */
class QtResource : public three::Resource, public streambuf
{
  friend class FileSystemLoader;

  const QByteArray _bytes;
  const char *_data;
  istream is;

  pos_type _pos = 0;

  static QByteArray readAll(QString name, ios_base::openmode openmode)
  {
    QFile file(name);
    if(!file.exists()) throw invalid_argument("cannot read file: "+name.toStdString());
    QFile::OpenMode flags;
    if(openmode & ios_base::app) flags |= QIODevice::Append;
    if(openmode & ios_base::ate) flags |= QIODevice::Append;
    if(!(openmode & ios_base::binary)) flags |= QIODevice::Text;
    if(openmode & ios_base::in) flags |= QIODevice::ReadOnly;
    if(openmode & ios_base::out) flags |= QIODevice::WriteOnly;
    if(openmode & ios_base::trunc) flags |= QIODevice::Truncate;
    file.open(flags);

    return file.readAll();
  }

  QtResource(QString fileName, ios_base::openmode openmode)
     : _bytes(readAll(fileName, openmode)), _data(_bytes.data()), is(this)
  {}

public:
  using Ptr = shared_ptr<QtResource>;

  static Ptr make(const QString &file, ios_base::openmode openmode) {
    return Ptr(new QtResource(file, openmode));
  }

  size_t size() override
  {
    return _bytes.size();
  }

  istream &in() override
  {
    return is;
  }

protected:
  pos_type seekoff(long off, ios_base::seekdir seekdir, ios_base::openmode openmode) override
  {
    if(openmode == ios_base::in) {
      switch(seekdir) {
        case ios_base::beg:
          _pos = off;
          break;
        case ios_base::cur:
          _pos += off;
          break;
        case ios_base::end:
          _pos = _bytes.size() - off;
          break;
      }
    }
    return _pos;
  }

  pos_type seekpos(pos_type pos, ios_base::openmode openmode) override
  {
    _pos = pos;
    return _pos;
  }

  streamsize showmanyc() override
  {
    return _bytes.size() - _pos;
  }

  streamsize xsgetn(char_type *s, streamsize count) override
  {
    streamsize off = min(count, _bytes.size() - _pos);
    memcpy(s, _data + _pos, off);
    _pos += off;
    return off;
  }

  int underflow() override
  {
    if(_pos == _bytes.size() - 1) return traits_type::eof();
    return _data[_pos];
  }

  int uflow() override
  {
    if(_pos == _bytes.size() - 1) return traits_type::eof();
    int ret = _data[_pos];
    _pos += 1;
    return ret;
  }

  int pbackfail(int c) override
  {
    if(_pos == 0) return traits_type::eof();
    return 1;
  }
};

/**
 * represents a resource loaded from the file system
 */
class FileSystemResource : public three::Resource
{
  friend class FileSystemLoader;

  fstream fin;
  QFileInfo info;

  FileSystemResource(const QString &path, ios_base::openmode openmode)
     : fin(path.toStdString(), openmode), info(path)
  {
    if(!info.exists() || fin.fail()) throw invalid_argument("cannot read file: "+path.toStdString());
  }

public:
  using Ptr = shared_ptr<FileSystemResource>;

  static Ptr make(const QString &path, ios_base::openmode openmode) {
    return Ptr(new FileSystemResource(path, openmode));
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

/**
 * loader that resolves resources from Qt resource storage
 */
class FileSystemLoader : public QThread, public three::ResourceLoader
{
Q_OBJECT

  loader::Assimp &assimp;
  QFileInfo file;
  QDir dir;

  unordered_map<string, string> _replacements;

public:
  FileSystemLoader(loader::Assimp &assimp, const QUrl &url, const unordered_map<string, string> &replacements)
     : assimp(assimp), file(url.toLocalFile()), dir(file.absoluteDir()), _replacements(replacements) {}

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
      string lookFor = path;
      auto repl = _replacements.find(lookFor);
      if(repl != _replacements.end()) lookFor = (*repl).second;

      return dir.exists(QString::fromStdString(lookFor));
    }
  }

  three::Resource::Ptr get(const char *path, ios_base::openmode openmode) override
  {
    if(file.fileName() == path) {
      return FileSystemResource::make(file.absoluteFilePath(), openmode);
    }
    else {
      string lookFor = path;
      auto repl = _replacements.find(lookFor);
      if(repl != _replacements.end()) lookFor = (*repl).second;

      return FileSystemResource::make(dir.absoluteFilePath(QString::fromStdString(lookFor)), openmode);
    }
  }

  void load(QImage &image, string &file) override
  {
    QString path = dir.absoluteFilePath(QString::fromStdString(file));
    QImage img(path);
    if(img.isNull())
      qWarning() << "error loading image from " << path;
    else
      image = img.mirrored();
  }

signals:
  void completed();
};

/**
 * loader that resolves resources from Qt resource storage
 */
class ResourceLoader : public QThread, public three::ResourceLoader
{
Q_OBJECT

  loader::Assimp &assimp;
  QFile file;

  unordered_map<string, string> _replacements;

public:
  ResourceLoader(loader::Assimp &assimp, const QString &url, const unordered_map<string, string> &replacements)
     : assimp(assimp), file(url), _replacements(replacements) {
    if(!file.exists()) qWarning() << "file " << file.fileName() << "does not exist";
  }

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
      string lookFor = path;
      auto repl = _replacements.find(lookFor);
      if(repl != _replacements.end()) lookFor = (*repl).second;

      QFile f(QString::fromStdString(lookFor));
      return f.exists();
    }
  }

  three::Resource::Ptr get(const char *path, ios_base::openmode openmode) override
  {
    if(file.fileName() == path) {
      return QtResource::make(file.fileName(), openmode);
    }
    else {
      string lookFor = path;
      auto repl = _replacements.find(lookFor);
      if(repl != _replacements.end()) lookFor = (*repl).second;

      return QtResource::make(QString::fromStdString(lookFor), openmode);
    }
  }

  void load(QImage &image, string &file) override
  {
    string lookFor = file;
    auto repl = _replacements.find(lookFor);
    if(repl != _replacements.end()) lookFor = (*repl).second;

    image = QImage(QString::fromStdString(lookFor)).mirrored();
  }

signals:
  void completed();
};

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
    QObject::connect(loader, &FileSystemLoader::completed, this, &Model::modelLoaded);
    loader->start();
  }
  else {
    ResourceLoader *loader = new ResourceLoader(*_assimp, file, replacements);
    QObject::connect(loader, &ResourceLoader::completed, this, &Model::modelLoaded);
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
  _container = container;

  if(!(_file.isNull() || _file.isEmpty())) loadFile(_file);
}

}
}

#include "Model.moc"
