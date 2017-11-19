//
// Created by byter on 11/4/17.
//

#ifndef THREE_QT_MESHTOONMATERIAL_H
#define THREE_QT_MESHTOONMATERIAL_H

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

  DefaultTexture::Ptr gradientMap;

protected:
  MeshToonMaterial() : MeshPhongMaterial(material::ResolverT<MeshToonMaterial>::make(*this)) {}

public:
  using Ptr = std::shared_ptr<MeshToonMaterial>;
  static Ptr make() {
    return Ptr(new MeshToonMaterial());
  }
};

}

#endif //THREE_QT_MESHTOONMATERIAL_H
