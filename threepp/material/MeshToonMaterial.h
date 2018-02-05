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
struct MeshToonMaterial : public MeshPhongMaterial
{
  std::unordered_map<std::string, std::string> defines {{ "TOON", "" }};

  Texture::Ptr gradientMap;

protected:
  MeshToonMaterial(const Color &color, bool dithering)
     : MeshPhongMaterial(material::ResolverT<MeshToonMaterial>::make(*this), color, dithering) {}

  MeshToonMaterial()
     : MeshPhongMaterial(material::ResolverT<MeshToonMaterial>::make(*this)) {}

public:
  using Ptr = std::shared_ptr<MeshToonMaterial>;
  static Ptr make(const Color &color, bool dithering) {
    return Ptr(new MeshToonMaterial(color, dithering));
  }

  static Ptr make() {
    return Ptr(new MeshToonMaterial());
  }
};

}

#endif //THREEPP_MESHTOONMATERIAL_H
