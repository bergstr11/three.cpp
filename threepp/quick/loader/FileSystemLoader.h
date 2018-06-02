//
// Created by byter on 2/11/18.
//

#ifndef THREE_PP_FILESYSTEMLOADER_H
#define THREE_PP_FILESYSTEMLOADER_H

#include <QThread>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <string>
#include <unordered_map>
#include <threepp/loader/Loader.h>

namespace three {
namespace quick {

/**
 * loader that resolves resources from the file system
 */
class FileSystemLoader : public QThread, public three::ResourceLoader
{
  Q_OBJECT

  three::Loader &loader;
  QFileInfo file;
  QDir dir;
  bool _loaded = false;

  std::unordered_map<std::string, std::string> _replacements;

protected:
  void run() override;

public:
  FileSystemLoader(three::Loader &assimp, const QUrl &url,
                   const std::unordered_map<std::string, std::string> &replacements=std::unordered_map<std::string, std::string>());

  bool exists(const char *path) override;

  three::Resource::Ptr get(const char *path, std::ios_base::openmode openmode) override;

  void load(QImage &image, const std::string &file) override;

private slots:
  void sendResult();

signals:
  void loaded();
};

}
}


#endif //THREE_PP_FILESYSTEMLOADER_H
