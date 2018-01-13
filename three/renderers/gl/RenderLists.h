//
// Created by byter on 12.09.17.
//

#ifndef THREE_QT_GLRENDERERLISTS_H
#define THREE_QT_GLRENDERERLISTS_H

#include <core/Object3D.h>
#include <core/Geometry.h>
#include <scene/Scene.h>
#include <camera/Camera.h>
#include "Program.h"

namespace three {
namespace gl {

struct RenderItem
{
  unsigned id;
  Object3D::Ptr object;
  BufferGeometry::Ptr geometry;
  Material::Ptr material;
  Program::Ptr program;
  int renderOrder;
  unsigned z;
  const Group *group;

  RenderItem(Object3D::Ptr object, BufferGeometry::Ptr geometry, Material::Ptr material, unsigned z, const Group *group,
             Program::Ptr program=nullptr)
     : object(object), geometry(geometry), material(material), program(program), renderOrder(object->renderOrder()),
       z(z), group(group)
  {}
};

class RenderList
{
  std::vector<RenderItem> _renderItems;

  std::vector<size_t> _opaque;
  std::vector<size_t> _transparent;

  bool painterSortStable(size_t index_a, size_t index_b)
  {
    const RenderItem &a = _renderItems.at(index_a);
    const RenderItem &b = _renderItems.at(index_b);

    if (a.renderOrder != b.renderOrder) {

      return a.renderOrder < b.renderOrder;
    }
    else if (a.program != nullptr && b.program != nullptr && a.program != b.program) {

      return a.program->handle() < b.program->handle();
    }
    else if (a.material->id != b.material->id) {

      return a.material->id < b.material->id;
    }
    else if (a.z != b.z) {

      return a.z < b.z;
    }
    else {

      return a.id < b.id;
    }
  }

  bool reversePainterSortStable(size_t index_a, size_t index_b)
  {
    const RenderItem &a = _renderItems.at(index_a);
    const RenderItem &b = _renderItems.at(index_b);

    if (a.renderOrder != b.renderOrder) {

      return a.renderOrder > b.renderOrder;
    }
    if (a.z != b.z) {

      return b.z > a.z;
    }
    else {

      return a.id > b.id;
    }
  }

public:
  class iterator
  {
    size_t _index;
    const std::vector<size_t> &_indizes;
    const std::vector<RenderItem> &_renderItems;

  public:
    typedef iterator self_type;
    typedef const RenderItem value_type;
    typedef const RenderItem &reference;
    typedef const RenderItem *pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;

    iterator(const std::vector<size_t> &indizes,
                const std::vector<RenderItem> &renderItems,
                size_t index=0)
       : _indizes(indizes), _renderItems(renderItems), _index(index)
    {}

    self_type operator++()
    {
      self_type i = *this;
      _index++;
      return i;
    }

    self_type operator++(int junk)
    {
      _index++;
      return *this;
    }

    const reference operator*()
    { return _renderItems[_indizes[_index]]; }

    const pointer operator->()
    { return &_renderItems[_indizes[_index]]; }

    bool operator==(const self_type &rhs)
    { return _index == rhs._index; }

    bool operator!=(const self_type &rhs)
    { return _index != rhs._index; }

    operator bool () {return _index < _indizes.size();}
  };

  void init()
  {
    _renderItems.clear();
    _opaque.clear();
    _transparent.clear();
  }

  RenderList &push_back(Object3D::Ptr object, BufferGeometry::Ptr geometry, Material::Ptr material, unsigned z, const Group *group)
  {
    _renderItems.emplace_back(object, geometry, material, z, group);

    if(material->transparent)
      _transparent.push_back(_renderItems.size() - 1);
    else
      _opaque.push_back(_renderItems.size() - 1);
    return *this;
  }

  RenderList &push_front(Object3D::Ptr object, BufferGeometry::Ptr geometry, Material::Ptr material, unsigned z, const Group *group)
  {
    _renderItems.emplace_back(object, geometry, material, z, group);

    if(material->transparent)
      _transparent.insert(_transparent.begin(), _renderItems.size() - 1);
    else
      _opaque.insert(_opaque.begin(), _renderItems.size() - 1);
    return *this;
  }

  iterator opaque() const {return iterator(_opaque, _renderItems);}

  iterator transparent() const {return iterator(_transparent, _renderItems);}

  RenderList &sort()
  {
    if (!_opaque.empty())
      std::sort(_opaque.begin(), _opaque.end(),
                [this](size_t a, size_t b) {return painterSortStable(a, b);});
    if (!_transparent.empty())
      std::sort(_transparent.begin(), _transparent.end(),
                [this](size_t a, size_t b) {return reversePainterSortStable(a, b);});
    return *this;
  }
};

class RenderLists
{
  std::unordered_map<uint32_t, RenderList> _lists;

public:
  RenderList *get(Scene::Ptr scene, Camera::Ptr camera)
  {
    uint32_t key = (uint32_t)scene->id() << 16 | camera->id();

    if(_lists.count(key) == 0) {
      _lists.emplace(key, RenderList());
    }
    return &_lists[key];
  }

  void dispose()
  {
    _lists.clear();
  }
};

}
}
#endif //THREE_QT_GLRENDERERLISTS_H
