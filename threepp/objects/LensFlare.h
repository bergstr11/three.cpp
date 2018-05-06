//
// Created by byter on 11.09.17.
//

#ifndef THREEPP_LENSFLARE_H
#define THREEPP_LENSFLARE_H

#include <threepp/core/Object3D.h>
#include <threepp/core/Color.h>
#include <threepp/textures/Texture.h>

namespace three {

class LensFlare : public Object3D
{
public:
  struct Flare {
    Texture::Ptr texture; // THREE.Texture
    size_t size;          // size in pixels (-1 = use texture.width)
    float distance;       // distance (0-1) from light source (0=at light source)
    float x, y, z;      // screen position (-1 => 1) z = 0 is in front z = 1 is back
    float scale;          // scale
    float rotation;       // rotation
    float opacity;        // opacity
    Color color;          // color
    Blending blending;    // blending

    Flare(Texture::Ptr texture, size_t size, float distance, unsigned int x, unsigned int y,
          unsigned int z, float scale, float rotation, float opacity, const Color &color,
          Blending blending) : texture(texture), size(size), distance(distance), x(x), y(y), z(z), scale(scale),
                               rotation(rotation), opacity(opacity), color(color), blending(blending)
    {}
  };

private:
  math::Vector3 _positionScreen;

  std::vector<Flare> _flares;

  LensFlare() : Object3D() {
    Object3D::typer = object::Typer(this);
  }

  LensFlare(const LensFlare &lf) : Object3D(lf), _flares(lf._flares) {
    Object3D::typer = object::Typer(this);
  }

public:
  using Ptr = std::shared_ptr<LensFlare>;
  static Ptr make(const Texture::Ptr texture, size_t size, float distance, Blending blending, const Color &color, float opacity)
  {
    Ptr l(new LensFlare());
    l->add(texture, size, distance, blending, color, opacity);
    return l;
  }

  std::function<void(LensFlare *)> customUpdateCallback;

  const std::vector<LensFlare::Flare> flares() const {return _flares;}

  LensFlare &add(Texture::Ptr texture, size_t size, float distance,
                 Blending blending=Blending::Normal, const Color &color=Color(0xffffff), float opacity=1.0f)
  {
    distance = std::min( distance, std::max( 0.0f, distance ) );

    _flares.emplace_back(texture, size, distance, 0, 0, 0, 1.0f, 0.0f, opacity,	color, blending);
    return *this;
  }

  math::Vector3 &positionScreen() {return _positionScreen;}

  void setScreenPosition(const math::Vector3 &screenPosition)
  {
    _positionScreen = screenPosition;

    if (customUpdateCallback) {
      customUpdateCallback(this);
    }
    else {
      updateLensFlares();
    }
  }

  /*
   * Update lens flares update positions on all flares based on the screen position
   * Set myLensFlare.customUpdateCallback to alter the flares in your project specific way.
   */
  LensFlare &updateLensFlares()
  {
    float vecX = - _positionScreen.x() * 2;
    float vecY = - _positionScreen.y() * 2;

    for (Flare &flare : _flares) {

      flare.x = _positionScreen.x() + vecX * flare.distance;
      flare.y = _positionScreen.y() + vecY * flare.distance;

      flare.rotation += ( flare.x * M_PI * 0.25 - flare.rotation ) * 0.25;
    }
    return *this;
  }

  LensFlare *cloned() const override {
    return object::Typer::set(new LensFlare(*this));
  }
};

}

#endif //THREEPP_LENSFLARE_H
