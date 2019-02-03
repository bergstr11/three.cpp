//
// Created by byter on 2/11/18.
//

#include "QtResourceLoader.h"

#include <threepp/util/impl/utils.h>

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
  { }

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
  pos_type seekoff(off_type off, ios_base::seekdir seekdir, ios_base::openmode openmode) override
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
    streamsize off = min((size_t)count, (size_t)(_bytes.size() - _pos));
    memcpy(s, _data + _pos, off);
    _pos += off;
    return off;
  }

  int underflow() override
  {
    if(_pos == (streamoff)(_bytes.size() - 1)) return traits_type::eof();
    return _data[_pos];
  }

  int uflow() override
  {
    if(_pos == (streamoff)(_bytes.size() - 1)) return traits_type::eof();
    int ret = _data[_pos];
    _pos += 1;
    return ret;
  }

  int pbackfail(int c) override
  {
    if(_pos == (streamoff)0) return traits_type::eof();
    return 1;
  }
};

QtResourceLoader::QtResourceLoader(three::Loader &loader, const QString &url,
                                   const unordered_map<string, string> &replacements) : loader(loader), file(url), _replacements(replacements) {
  if (!file.exists())
    qWarning() << "file " << file.fileName() << "does not exist";
  else
    QObject::connect(this, &QThread::finished, this, &QtResourceLoader::sendResult, Qt::QueuedConnection);
}

void QtResourceLoader::run()
{
  loader.load(file.fileName().toStdString(), *this);
  _loaded = true;
}

bool QtResourceLoader::exists(const char *path)
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

three::Resource::Ptr QtResourceLoader::get(const char *path, ios_base::openmode openmode)
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

void QtResourceLoader::load(QImage &image, const string &file)
{
  //fix spurious backslashes
  auto f = three::replace_all(file, "\\", "/");

  string &lookFor = f;
  auto repl = _replacements.find(lookFor);
  if(repl != _replacements.end()) lookFor = (*repl).second;

  QString name = _resourcePrefix + QString::fromStdString(lookFor);
  image = QImage(name).mirrored();
}

void QtResourceLoader::sendResult()
{
  if(_loaded) emit loaded();
  deleteLater();
}

}
}