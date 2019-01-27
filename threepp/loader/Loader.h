//
// Created by byter on 1/6/18.
//

#ifndef THREEPP_LOADER_H
#define THREEPP_LOADER_H

#include <istream>
#include <unordered_map>
#include <threepp/util/simplesignal.h>
#include <threepp/util/Typer.h>
#include <threepp/objects/Node.h>
#include <threepp/scene/Scene.h>

#include <tuple>

namespace three {

struct Resource
{
  virtual size_t size() = 0;
  virtual std::istream &in() = 0;
  virtual ~Resource() = default;
  using Ptr = std::shared_ptr<Resource>;
};

struct ResourceLoader
{
  virtual bool exists(const char *path) = 0;
  virtual void load(QImage &image, const std::string &file) = 0;
  virtual Resource::Ptr get(const char *path, std::ios_base::openmode) = 0;
};

namespace loader {

/**
 * MaterialHandler is a variadic template class which provides a mechanism by which the application
 * can intervene into the model loading process with the goal of enhancing or replacing loaded materials.
 * It may or may not be supported by actual loaders
 *
 * @tparam M
 * @tparam Ms
 */
template <typename... Ms>
struct MaterialHandler;

template <typename M>
struct MaterialHandler<M>
{
  const unsigned index;

protected:
  MaterialHandler(unsigned index) : index(index) {}
  MaterialHandler() : index(0) {}

public:
  virtual void handle(const std::string &name, M &m, Material::Ptr mp) const = 0;

  void _handle(const std::string &name, M &m, Material::Ptr mp) const
  {
    //another indirection - required to make the virtual specifier above work
    handle(name, m, mp);
  }
};

template <typename M, typename... Ms>
struct MaterialHandler<M, Ms...> : MaterialHandler<M>, MaterialHandler<Ms...>
{
  MaterialHandler() : MaterialHandler<M>(sizeof...(Ms)) {}
  virtual ~MaterialHandler() {}

  /**
   * handle a material previously created by the loader as the result of a #createAs
   * call. After the material object is created, the loader will load all applicable data
   * into it and then pass it on to this function. The application has the opportunity
   * to enhance the material (e.g., provide an environment map)
   *
   * @tparam Mx the material type
   * @param name the material name
   * @param mx the material as actual type
   * @param mp the material as generic pointer
   */
  template <typename Mx>
  void handle(const std::string &name, Mx &mx, Material::Ptr mp) const
  {
    //disambiguate
    MaterialHandler<Mx>::_handle(name, mx, mp);
  }

  /**
   * determine that a given named material should be created as the given type (application side)
   *
   * @tparam Mx the material type
   * @param name the material name
   */
  template <typename Mx> void createAs(const std::string &name)
  {
    type_map[name] = MaterialHandler<Mx>::index;
  }

  /**
   * return whether the material with the given name should be created as the given type (loader side)
   *
   * @tparam Mx the material type
   * @param name the material name
   * @return true if the material should be created as the given type
   */
  template <typename Mx> bool create(const std::string &name) const
  {
    auto found = type_map.find(name);
    if(found != type_map.end()) {
      return found->second == MaterialHandler<Mx>::index;
    }
    return false;
  }

private:
  MaterialHandler(const MaterialHandler &h) = delete;

  std::unordered_map<std::string, unsigned> type_map;
};

}

/**
 * abstract superclass for model loaders
 */
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
