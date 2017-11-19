//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_MATERIAL_H
#define THREE_QT_MATERIAL_H

#include <memory>
#include <Constants.h>
#include <textures/Texture.h>
#include <helper/sole.h>
#include <helper/simplesignal.h>
#include <math/Plane.h>
#include <helper/Resolver.h>
#include <core/Color.h>

namespace three {

class Camera;

namespace material {

struct Colored {
  Color color {0xffffff};
  float opacity = 1;
};

struct LightMap
{
  DefaultTexture::Ptr lightMap;
  float lightMapIntensity = 1.0;
};

struct EmissiveMap
{
  DefaultTexture::Ptr emissiveMap;
  Color emissive = 0x000000;
  float emissiveIntensity = 1.0;
};

struct AoMap
{
  DefaultTexture::Ptr aoMap;
  float aoMapIntensity = 1.0;
};

struct EnvMap
{
  DefaultTexture::Ptr envMap;
  CombineOperation combine = CombineOperation::Multiply;
  float reflectivity = 1;
  float refractionRatio = 0.98;
};

struct AlphaMap
{
  DefaultTexture::Ptr alphaMap;
};

struct SpecularMap
{
  DefaultTexture::Ptr specularMap;
};

struct DisplacementMap
{
  DefaultTexture::Ptr displacementMap;
  float displacementScale = 1;
  float displacementBias = 0;
};

struct BumpMap
{
  DefaultTexture::Ptr bumpMap;
  float bumpScale = 1;
};

struct NormalMap
{
  DefaultTexture::Ptr normalMap;
  math::Vector2 normalScale {1, 1};
};

}

struct Material
{
  const sole::uuid uuid;
  uint16_t id;

  bool fog = true;
  bool lights = true;

  Blending blending = Blending::Normal;
  Side side = Side::Front;
  bool flatShading = false;
  Colors vertexColors = Colors::None;

  bool transparent = false;

  BlendFunc blendSrc = BlendFunc::SrcAlpha;
  BlendFunc blendDst = BlendFunc::OneMinusSrcAlpha;
  BlendEq blendEquation = BlendEq::Add;

  BlendFunc blendSrcAlpha = BlendFunc::None;
  BlendFunc blendDstAlpha = BlendFunc::None;
  BlendEq blendEquationAlpha = BlendEq::None;

  Func depthFunc = Func::LessEqual;
  bool depthTest = true;
  bool depthWrite = true;

  std::vector<math::Plane> clippingPlanes;
  bool clipIntersection = false;
  bool clipShadows = false;

  bool colorWrite = true;

  bool wireframe = false;
  unsigned wireframeLineWidth;
  LineCap wireframeLineCap = LineCap::round;
  LineJoin wireframeLineJoin = LineJoin::round;

  Precision precision = Precision::unknown; // override the renderer's default precision for this material

  bool polygonOffset = false;
  unsigned polygonOffsetFactor = 0;
  unsigned polygonOffsetUnits = 0;

  bool dithering = false;

  float alphaTest = 0;
  bool premultipliedAlpha = false;

  DefaultTexture::Ptr map;

  bool skinning = false;
  bool morphTargets = false;
  unsigned numSupportedMorphTargets = 0;
  bool morphNormals = false;
  unsigned numSupportedMorphNormals = 0;

  unsigned overdraw = 0; // Overdrawn pixels (typically between 0 and 1) for fixing antialiasing gaps in CanvasRenderer

  bool visible = true;

  bool needsUpdate = true;

  material::Resolver::Ptr resolver;

  Signal<void(Material *)> onDispose;

protected:
  Material(material::Resolver::Ptr resolver) : uuid(sole::uuid0()), resolver(resolver) {}
  Material() : uuid(sole::uuid0()), resolver(material::ResolverT<Material>::make(*this)) {}

public:
  virtual ~Material() {}

  using Ptr = std::shared_ptr<Material>;

  virtual void setupPointLight(const math::Vector3 &position, float near, float far)
  {
  }
};

template <typename ... Maps>
struct MaterialT;

template<typename Map>
struct MaterialT<Map> : public Material, public Map
{
protected:
  MaterialT(material::Resolver::Ptr resolver) : Material(resolver) {}
  MaterialT() : Material() {}
};

template<>
struct MaterialT<> : public Material
{
protected:
  MaterialT(material::Resolver::Ptr resolver) : Material(resolver) {}
  MaterialT() : Material() {}
};

template<typename Map, typename ... Maps>
struct MaterialT<Map, Maps...> : public Material, Map, Maps...
{
protected:
  MaterialT(material::Resolver::Ptr resolver) : Material(resolver) {}
  MaterialT() : Material() {}
};

}
#endif //THREE_QT_MATERIAL_H
