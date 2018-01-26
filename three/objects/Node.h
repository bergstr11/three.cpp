//
// Created by byter on 1/7/18.
//

#ifndef THREEPP_NODE_H
#define THREEPP_NODE_H

#include <core/Object3D.h>

namespace three {

/**
 * a non-renderable scenegraph node
 */
class Node : public Object3D
{
protected:
  Node(std::string name) : Object3D(object::ResolverT<Node>::make(*this)) {
    _name = name;
  }

public:
  using Ptr = std::shared_ptr<Node>;
  static Ptr make(std::string name="") {
    return Ptr(new Node(name));
  }

  bool isEmpty() {return _children.empty();}
};

}

#endif //THREEPP_NODE_H
