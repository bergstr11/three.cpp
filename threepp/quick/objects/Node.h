//
// Created by byter on 3/18/18.
//

#ifndef THREE_PPQ_NODE_H
#define THREE_PPQ_NODE_H

#include <threepp/objects/Node.h>
#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/objects/ThreeQObject.h>

namespace three {
namespace quick {

class Node : public ThreeQObject
{
  Q_OBJECT

  three::Node::Ptr _node;

protected:
  Object3D::Ptr _create() override
  {
    if(!_node) _node = three::Node::make(_name.toStdString());
    return _node;
  }

public:
  Node(QObject *parent=nullptr) : ThreeQObject(parent) {}
  Node(three::Node::Ptr node, QObject *parent=nullptr)
     : ThreeQObject(node, parent), _node(node) {}
};

}
}

#endif //THREE_PPQ_NODE_H
