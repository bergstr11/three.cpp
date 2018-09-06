//
// Created by byter on 2/11/18.
//

#include "FileSystemLoader.h"
#include <fstream>

#include <threepp/util/impl/utils.h>

namespace three {
namespace quick {

using namespace std;

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

FileSystemLoader::FileSystemLoader(three::Loader &loader, const QFileInfo &info, const unordered_map<std::string, std::string> &replacements)
: _loader(loader), _file(info), _dir(info.absoluteDir()), _replacements(replacements)
{
  QObject::connect(this, &QThread::finished, this, &FileSystemLoader::sendResult, Qt::QueuedConnection);
}

void FileSystemLoader::run()
{
  _loader.load(_file.canonicalFilePath().toStdString(), *this);
  _loaded = true;
}

bool FileSystemLoader::exists(const char *path)
{
  if(_file.fileName() == path) {
    return _file.exists();
  }
  else {
    string lookFor = path;
    auto repl = _replacements.find(lookFor);
    if(repl != _replacements.end()) lookFor = (*repl).second;

    return _dir.exists(QString::fromStdString(lookFor));
  }
}

void FileSystemLoader::sendResult()
{
  if(_loaded) emit loaded();
  deleteLater();
}

three::Resource::Ptr FileSystemLoader::get(const char *path, ios_base::openmode openmode)
{
  if(_file.fileName() == path) {
    return FileSystemResource::make(_file.absoluteFilePath(), openmode);
  }
  else {
    string lookFor = path;
    auto repl = _replacements.find(lookFor);
    if(repl != _replacements.end()) lookFor = (*repl).second;

    return FileSystemResource::make(_dir.absoluteFilePath(QString::fromStdString(lookFor)), openmode);
  }
}

void FileSystemLoader::load(QImage &image, const string &file)
{
  //fix spurious backslashes
  auto f = three::replace_all(file, "\\", "/");

  QString path = _dir.absoluteFilePath(QString::fromStdString(f));
  QImage img(path);

  if(!img.isNull()) image = img.mirrored();
}

}
}