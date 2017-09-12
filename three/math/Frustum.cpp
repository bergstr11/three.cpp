//
// Created by byter on 11.09.17.
//

#include "Frustum.h"

#include <objects/Sprite.h>

namespace three {
namespace math {

bool Frustum::intersectsObject(const std::shared_ptr<Object3D> object)
{
  if (object->geometry()->boundingSphere().isEmpty())
    object->geometry()->computeBoundingSphere();

  Sphere sphere = object->geometry()->boundingSphere();
  sphere.apply(object->matrixWorld());

  return intersectsSphere( sphere );
}

bool Frustum::intersectsSprite(const std::shared_ptr<Sprite> sprite)
{
  Sphere sphere(Vector3(0, 0, 0), 0.7071067811865476);
  sphere.apply(sprite->matrixWorld());

  return intersectsSphere( sphere );
}


}
}