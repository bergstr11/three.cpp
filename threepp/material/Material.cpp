//
// Created by byter on 26.09.17.
//

#include <threepp/camera/Camera.h>
#include "MeshBasicMaterial.h"
#include "MeshDistanceMaterial.h"
#include "MeshDepthMaterial.h"
#include "MeshLambertMaterial.h"
#include "MeshNormalMaterial.h"
#include "MeshPhongMaterial.h"
#include "MeshStandardMaterial.h"
#include "MeshToonMaterial.h"
#include "PointsMaterial.h"

namespace three {


void MeshDistanceMaterial::setupPointLight(const math::Vector3 &position, float near, float far)
{
  referencePosition = position;
  nearDistance = near;
  farDistance = far;
}

template <>
struct MixinResolve<>;

template <typename Mixin, typename ... Mixins>
struct MixinResolve<Mixin, Mixins...>
{
  template <typename ... Ms>
  static void mixin(MaterialT<Ms...> &material, const material::Selector & selector) {
    MixinResolve<Mixin>::mixin(material, selector);
    MixinResolve<Mixins...>::mixin(material, selector);
  }
  static void handle(MaterialT<Mixin, Mixins...> &material, const material::Selector & selector) {
    MixinResolve<Mixin>::mixin(material, selector);
    MixinResolve<Mixins...>::mixin(material, selector);
  }
};

using namespace material;

#define MIXIN_RESOLVE(Mixin, field) template <> \
struct MixinResolve<Mixin> {\
  static void handle(MaterialT<Mixin> &material, const Selector & selector) { \
    mixin(material, selector); \
  } \
  static void mixin(Mixin &mixin, const Selector & selector) { \
    if(selector.field) selector.field(mixin); \
  } \
};

MIXIN_RESOLVE(Colored, colored)
MIXIN_RESOLVE(LightMap, lightMap)
MIXIN_RESOLVE(Emissive, emissive)
MIXIN_RESOLVE(AoMap, aoMap)
MIXIN_RESOLVE(EnvMap, envMap)
MIXIN_RESOLVE(AlphaMap, alphaMap)
MIXIN_RESOLVE(SpecularMap, specularMap)
MIXIN_RESOLVE(Specular, specular)
MIXIN_RESOLVE(DisplacementMap, displacementMap)
MIXIN_RESOLVE(BumpMap, bumpMap)
MIXIN_RESOLVE(NormalMap, normalMap)
MIXIN_RESOLVE(RoughnessMap, roughnessMap)
MIXIN_RESOLVE(MetalnessMap, metalnessMap)

void MeshBasicMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void MeshDistanceMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void MeshDepthMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void MeshLambertMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void MeshNormalMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void MeshPhongMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void MeshStandardMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void MeshToonMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}
void PointsMaterial::callback(const material::Selector &selector)
{
  ResolveT::handle(*this, selector);
}

Material::Material(const Material &material, material::Resolver::Ptr resolver)
   : uuid(sole::uuid0()), id(___material_id_count++), resolver(resolver)
{
  fog = material.fog;
  lights = material.lights;

  ambientColor = material.ambientColor;

  blending = material.blending;
  side = material.side;
  flatShading = material.flatShading;
  vertexColors = material.vertexColors;

  transparent = material.transparent;

  blendSrc = material.blendSrc;
  blendDst = material.blendDst;
  blendEquation = material.blendEquation;

  blendSrcAlpha = material.blendSrcAlpha;
  blendDstAlpha = material.blendDstAlpha;
  blendEquationAlpha = material.blendEquationAlpha;

  depthFunc = material.depthFunc;
  depthTest = material.depthTest;
  depthWrite = material.depthWrite;

  clippingPlanes = material.clippingPlanes;
  clipIntersection = material.clipIntersection;
  clipShadows = material.clipShadows;

  colorWrite = material.colorWrite;

  wireframe = material.wireframe;
  wireframeLineWidth = material.wireframeLineWidth;
  wireframeLineCap = material.wireframeLineCap;
  wireframeLineJoin = material.wireframeLineJoin;

  precision = material.precision;

  polygonOffset = material.polygonOffset;
  polygonOffsetFactor = material.polygonOffsetFactor;
  polygonOffsetUnits = material.polygonOffsetUnits;

  dithering = material.dithering;

  alphaTest = material.alphaTest;
  premultipliedAlpha = material.premultipliedAlpha;

  map = material.map;

  skinning = material.skinning;
  morphTargets = material.morphTargets;
  numSupportedMorphTargets = material.numSupportedMorphTargets;
  morphNormals = material.morphNormals;
  numSupportedMorphNormals = material.numSupportedMorphNormals;

  overdraw = material.overdraw;

  visible = material.visible;

  needsUpdate = material.needsUpdate;
}

}