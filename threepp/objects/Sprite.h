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

class Sprite : public Object3D
{
  explicit Sprite(BufferGeometry::Ptr geometry, SpriteMaterial::Ptr material)
     : Object3D(geometry, material)
  {
    Object3D::typer = object::Typer(this);
  }

  Sprite(const Sprite &sprite)
     : Object3D(sprite)
  {
    Object3D::typer = object::Typer(this);
  }

public:
  using Ptr = std::shared_ptr<Sprite>;
  static Ptr make(SpriteMaterial::Ptr material = SpriteMaterial::make()) {
    return Ptr(new Sprite(nullptr, material));
  }

  void raycast(Raycaster raycaster, std::vector<Intersect> &intersects)
  {
    math::Vector3 intersectPoint;

    math::Vector3 worldPosition = _matrixWorld.getPosition();

    for(const auto &ray : raycaster.rays()) {

      intersectPoint = ray.closestPointToPoint(worldPosition);

      math::Vector3 worldScale = math::Vector3::fromMatrixScale(_matrixWorld);
      float guessSizeSq = worldScale.x() * worldScale.y() / 4;

      if (worldPosition.distanceToSquared(intersectPoint) > guessSizeSq) return;

      float distance = raycaster.origin().distanceTo(intersectPoint);

      if (distance < raycaster.near() || distance > raycaster.far()) return;

      intersects.emplace_back(distance, intersectPoint, nullptr, *this);
    }
  }

  Sprite *cloned() const override
  {
    return new Sprite(*this);
  }
};

}
#endif //THREEPP_SPRITE_H
