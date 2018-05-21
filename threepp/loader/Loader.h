//
// Created by byter on 1/6/18.
//

#ifndef THREEPP_LOADER_H
#define THREEPP_LOADER_H

#include <istream>
#include <threepp/util/simplesignal.h>
#include <threepp/objects/Node.h>
#include <threepp/scene/Scene.h>

namespace three {

struct Resource
{
  virtual size_t size() = 0;
  virtual std::istream &in() = 0;
  virtual ~Resource() = default;
  using Ptr = std::shared_ptr<Resource>;
};

struct ResourceLoader {
  virtual bool exists(const char *path) = 0;
  virtual void load(QImage &image, std::string &file) = 0;
  virtual Resource::Ptr get(const char *path, std::ios_base::openmode) = 0;
};

class Loader
{
protected:
  Scene::Ptr _scene;

public:
  using Ptr = std::shared_ptr<Loader>;

  virtual ~Loader() = default;

  Signal<void(std::string message)> onError;

  virtual void load(std::string name, ResourceLoader &loader) = 0;
  virtual void load(std::string name, const Color &background, ResourceLoader &loader) = 0;
  virtual void load(std::string name, Texture::Ptr background, ResourceLoader &loader) = 0;

  const Scene::Ptr scene() const {return _scene;}
};

}


#endif //THREEPP_LOADER_H
