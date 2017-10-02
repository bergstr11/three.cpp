//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_PROPERTIES_H
#define THREE_QT_PROPERTIES_H

#include <unordered_map>
#include <string>
#include <core/Object3D.h>

namespace three {
namespace gl {

class Properties
{
  std::unordered_map<sole::uuid, std::unordered_map<std::string, std::string>> properties = {};

public:
  std::unordered_map<std::string, std::string> get(const Object3D::Ptr object)
  {
    //var uuid = object->uuid;
    if(properties.find(object->uuid) == properties.end())
      properties.emplace(object->uuid, std::unordered_map<std::string, std::string>());

    return properties[object->uuid];
  }

  void remove(const Object3D::Ptr object)
  {
    properties.erase(object->uuid);
  }

  void clear()
  {
    properties.clear();
  }
};

}
}
#endif //THREE_QT_PROPERTIES_H
