//
// Created by byter on 5/1/18.
//

#include "ConvexHull.h"
#define QUICKHULL_IMPLEMENTATION
#include <threepp/util/3d-quickhull.h>

namespace three {

using namespace std;

ConvexHull::ConvexHull(const Object3D::Ptr &object) : _object(object)
{
  _visible = false;

  mesh = new qh_mesh_t();
  //qh_quickhull3d(*mesh, vertices, n);
}

ConvexHull::~ConvexHull()
{
  qh_free_mesh(*mesh);
}

void ConvexHull::raycast(const Raycaster &raycaster, std::vector<Intersection> &intersects)
{

}

}