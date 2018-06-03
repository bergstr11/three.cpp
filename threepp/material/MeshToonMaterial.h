//
// Created by byter on 11/4/17.
//

#ifndef THREEPP_MESHTOONMATERIAL_H
#define THREEPP_MESHTOONMATERIAL_H

#include "MeshPhongMaterial.h"

namespace three {

/**
 * @author takahirox / http://github.com/takahirox
 *
 * parameters = {
 *  gradientMap: new THREE.Texture( <Image> )
 * }
 */
struct DLX MeshToonMaterial : public MeshPhongMaterial
{
  std::unordered_map<std::string, std::string> defines {{ "TOON", "" }};

  Texture::Ptr gradientMap;

protected:
  MeshToonMaterial(const Color &color, bool dithering)
     : MeshPhongMaterial(material::InfoT<MeshToonMaterial>(), material::Typer(this), color, dithering) {}

  MeshToonMaterial()
     : MeshPhongMaterial(material::InfoT<MeshToonMaterial>(), material::Typer(this)) {}

  MeshToonMaterial(const MeshToonMaterial &material)
     : MeshPhongMaterial(material, material::InfoT<MeshToonMaterial>(), material::Typer(this)) {}

  void callback(const material::Selector &selector) override;

public:
  using Ptr = std::shared_ptr<MeshToonMaterial>;
  static Ptr make(const Color &color, bool dithering) {
    return Ptr(new MeshToonMaterial(color, dithering));
  }

  static Ptr make() {
    return Ptr(new MeshToonMaterial());
  }

  MeshToonMaterial *cloned() const override {
    return new MeshToonMaterial(*this);
  }
};

}

#endif //THREEPP_MESHTOONMATERIAL_H
