//
// Created by byter on 03.09.17.
//

#ifndef THREEPP_SPRITE_H
#define THREEPP_SPRITE_H

#include <threepp/core/Object3D.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/material/SpriteMaterial.h>

namespace three {

struct Intersect
{
  const float distance;
  const math::Vector3 intersectPoint;
  const Face3 face;
  const Object3D &object;

  Intersect(const float distance, const math::Vector3 &intersectPoint, const Face3 *face, const Object3D &object)
     : distance(distance), intersectPoint(intersectPoint), face(*face), object(object)
  {}
};

class Sprite : public Object3D_GM<BufferGeometry, SpriteMaterial>
{
  explicit Sprite(SpriteMaterial::Ptr material)
     : Object3D(object::ResolverT<Sprite>::make(*this)), Object3D_GM(nullptr, material)
  {}

public:
  using Ptr = std::shared_ptr<Sprite>;
  static Ptr make(SpriteMaterial::Ptr material = SpriteMaterial::make()) {
    return Ptr(new Sprite(material));
  }

  void raycast(Raycaster raycaster, std::vector<Intersect> &intersects)
  {
    math::Vector3 intersectPoint;

    math::Vector3 worldPosition = math::Vector3::fromMatrixPosition(_matrixWorld);
    intersectPoint = raycaster.ray().closestPointToPoint(worldPosition);

    math::Vector3 worldScale = math::Vector3::fromMatrixScale(_matrixWorld);
    float guessSizeSq = worldScale.x() * worldScale.y() / 4;

    if (worldPosition.distanceToSquared(intersectPoint) > guessSizeSq) return;

    float distance = raycaster.ray().origin().distanceTo(intersectPoint);

    if (distance < raycaster.near() || distance > raycaster.far()) return;

    intersects.emplace_back(distance, intersectPoint, nullptr, *this);
  }
};

}
#endif //THREEPP_SPRITE_H
