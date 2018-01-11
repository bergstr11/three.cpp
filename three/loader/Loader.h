//
// Created by byter on 1/6/18.
//

#ifndef THREEPP_LOADER_H
#define THREEPP_LOADER_H

#include <istream>
#include <helper/simplesignal.h>
#include <objects/Objects.h>
#include <scene/Scene.h>

namespace three {

struct ResourceLoader {
  virtual bool exists(const char *path) = 0;
  virtual std::istream &istream(const char *path, std::ios_base::openmode) = 0;
  virtual std::istream &istream(const std::string &path, std::ios_base::openmode) = 0;
};

class Loader
{
protected:
  Loader() {}

  Scene::Ptr _scene;
  Objects::Ptr _objects;

public:
  using Ptr = std::shared_ptr<Loader>;

  virtual ~Loader() {}

  Signal<void(Objects::Ptr objects)> onLoaded;
  Signal<void(unsigned percent)> onProgress;
  Signal<void(std::string message)> onError;

  virtual void load(std::string name, ResourceLoader &loader) = 0;
  virtual void load(std::string name, Color background, ResourceLoader &loader) = 0;
  virtual void load(std::string name, Texture::Ptr background, ResourceLoader &loader) = 0;

  const Scene::Ptr scene() const {return _scene;}
  const Objects::Ptr objects() {return _objects;}
};

}


#endif //THREEPP_LOADER_H
