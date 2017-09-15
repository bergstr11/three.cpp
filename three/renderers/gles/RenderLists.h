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
  Geometry::Ptr geometry;
  Material::Ptr material;
  Program::Ptr program;
  int renderOrder;
  unsigned z;
  int group;

  RenderItem(Object3D::Ptr object, Geometry::Ptr geometry, Material::Ptr material, unsigned z, int group, Program::Ptr program=nullptr)
     : object(object), geometry(geometry), material(material), program(program), renderOrder(object->renderOrder()), z(z), group(group)
  {}
};

int painterSortStable(const RenderItem &a, const RenderItem &b)
{
  if (a.renderOrder != b.renderOrder) {

    return a.renderOrder - b.renderOrder;
  }
  else if (a.program != nullptr && b.program != nullptr && a.program != b.program) {

    return a.program->id() - b.program->id();
  }
  else if (a.material->id() != b.material->id()) {
    return a.material->id() - b.material->id();
  }
  else if (a.z != b.z) {
    return a.z - b.z;
  }
  else {
    return a.id - b.id;
  }
}

int reversePainterSortStable(const RenderItem &a, const RenderItem &b)
{
  if (a.renderOrder != b.renderOrder) {

    return a.renderOrder - b.renderOrder;
  }
  if (a.z != b.z) {

    return b.z - a.z;
  }
  else {

    return a.id - b.id;
  }
}

class RenderList
{
  std::vector<RenderItem> renderItems;
  size_t renderItemsIndex = 0;

  std::vector<size_t> opaque;
  std::vector<size_t> transparent;

public:
  void init()
  {

    renderItemsIndex = 0;

    opaque.clear();
    transparent.clear();

  }

  RenderList &push_back(Object3D::Ptr object, Geometry::Ptr geometry, Material::Ptr material, unsigned z, int group)
  {

    renderItems.emplace_back(object, geometry, material, z, group);

    if(material->transparent)
      transparent.push_back(renderItems.size() - 1);
    else
      opaque.push_back(renderItems.size() - 1);
  }

  RenderList &sort()
  {
    if (!opaque.empty())
      std::sort(opaque.begin(), opaque.end(), painterSortStable);
    if (!transparent.empty())
      std::sort(transparent.begin(), transparent.end(), reversePainterSortStable);
  }
};

class RenderLists
{

  std::unordered_map<uint32_t, RenderList> _lists;

public:
  RenderList *get(Scene::Ptr scene, Camera::Ptr camera)
  {
    uint32_t key = (uint32_t)scene->id() << 16 | camera->id();

    if(_lists.find(key) == _lists.end()) {
      _lists.emplace(_lists.end(), key);
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
