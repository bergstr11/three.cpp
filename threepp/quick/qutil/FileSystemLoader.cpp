//
// Created by byter on 2/11/18.
//

#include "FileSystemLoader.h"
#include <fstream>

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

FileSystemLoader::FileSystemLoader(three::Loader &loader, const QUrl &url, const unordered_map<std::string, std::string> &replacements)
: loader(loader), file(url.toLocalFile()), dir(file.absoluteDir()), _replacements(replacements) {}

void FileSystemLoader::run()
{
  loader.load(file.fileName().toStdString(), *this);
  emit loaded();
}

bool FileSystemLoader::exists(const char *path)
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

three::Resource::Ptr FileSystemLoader::get(const char *path, ios_base::openmode openmode)
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

void FileSystemLoader::load(QImage &image, string &file)
{
  QString path = dir.absoluteFilePath(QString::fromStdString(file));
  QImage img(path);
  if(img.isNull())
    qWarning() << "error loading image from " << path;
  else
    image = img.mirrored();
}

}
}