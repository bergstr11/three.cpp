//
// Created by byter on 5/1/18.
//

#include "ConvexHull.h"
#include <threepp/util/quickhull.h>
#include <threepp/objects/Node.h>
#include <sstream>
#include <iostream>

namespace three {

using namespace std;

ConvexHull::ConvexHull(const std::vector<Object3D::Ptr> &objects) : _objects(objects)
{
  _visible = false;

  Node::Ptr node = Node::make(objects);
  QuickHull quickhull(node);
}

void ConvexHull::raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects)
{

}

}