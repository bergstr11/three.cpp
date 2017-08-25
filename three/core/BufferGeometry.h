//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_BUFFERGEOMETRY_H
#define THREE_QT_BUFFERGEOMETRY_H

#include <unordered_map>
#include "Geometry.h"
#include "BufferAttribute.h"

namespace three {

class BufferGeometry : public Geometry
{
  BufferAttribute<uint32_t> _index;
  std::unordered_map<std::string, BufferAttributeBase::Ptr> _attributes;
  /*
  this.morphAttributes = {};

  this.groups = [];

  this.drawRange = { start: 0, count: Infinity };
*/
public:
  BufferGeometry() : _index(3) {

  }

  void setIndex(const std::vector<uint32_t> &indices) {
    _index.set(indices);
  }

  BufferGeometry &addAttribute(std::string name, const BufferAttributeBase::Ptr &attribute)
  {
    _attributes[ name ] = attribute;

    return *this;
  }

  const BufferAttributeBase::Ptr &getAttribute(const std::string name) const
  {
    return _attributes.at(name);
  }

  BufferGeometry &removeAttribute(std::string name)
  {
    _attributes.erase(name);

    return *this;
  }
};

}

#endif //THREE_QT_BUFFERGEOMETRY_H
