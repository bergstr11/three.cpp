//
// Created by byter on 22.08.17.
//

#ifndef THREEPP_MATERIAL_H
#define THREEPP_MATERIAL_H

#include <memory>
#include <threepp/util/osdecl.h>
#include <threepp/Constants.h>
#include <threepp/textures/Texture.h>
#include <threepp/util/sole.h>
#include <threepp/util/simplesignal.h>
#include <threepp/math/Plane.h>
#include <threepp/util/Resolver.h>
#include <threepp/core/Color.h>

namespace three {

class Camera;

namespace material {

struct Colored {
  using Selector = std::function<void(Colored &)>;
  Color color {0xffffff}; //diffuse, really
  float opacity = 1;
};

struct LightMap
{
  using Selector = std::function<void(LightMap &)>;
  Texture::Ptr lightMap;
  float lightMapIntensity = 1.0;
};

struct Emissive
{
  using Selector = std::function<void(Emissive &)>;
  Texture::Ptr emissiveMap;
  Color emissive {0x000000};
  float emissiveIntensity = 1.0;
};

struct AoMap
{
  using Selector = std::function<void(AoMap &)>;
  Texture::Ptr aoMap;
  float aoMapIntensity = 1.0;
};

struct EnvMap
{
  using Selector = std::function<void(EnvMap &)>;
  Texture::Ptr envMap;
  CombineOperation combine = CombineOperation::Multiply;
  float reflectivity = 1;
  float refractionRatio = 0.98;
  float envMapIntensity = 1.0f;
};

struct AlphaMap
{
  using Selector = std::function<void(AlphaMap &)>;
  Texture::Ptr alphaMap;
};

struct SpecularMap
{
  using Selector = std::function<void(SpecularMap &)>;
  Texture::Ptr specularMap;
};

struct Specular : public SpecularMap
{
  using Selector = std::function<void(Specular &)>;
  float shininess = 30;
  Color specular {0x111111};
};

struct DisplacementMap
{
  using Selector = std::function<void(DisplacementMap &)>;
  Texture::Ptr displacementMap;
  float displacementScale = 1;
  float displacementBias = 0;
};

struct BumpMap
{
  using Selector = std::function<void(BumpMap &)>;
  Texture::Ptr bumpMap;
  float bumpScale = 1;
};

struct NormalMap
{
  using Selector = std::function<void(NormalMap &)>;
  Texture::Ptr normalMap;
  math::Vector2 normalScale {1, 1};
};

struct RoughnessMap
{
  using Selector = std::function<void(RoughnessMap &)>;
  float roughness = 0.5f;
  Texture::Ptr roughnessMap;
};

struct MetalnessMap
{
  using Selector = std::function<void(MetalnessMap &)>;
  float metalness = 0.5f;
  Texture::Ptr metalnessMap;
};

struct Selector
{
  Colored::Selector colored;
  LightMap::Selector lightMap;
  Emissive::Selector emissive;
  AoMap::Selector aoMap;
  AlphaMap::Selector alphaMap;
  EnvMap::Selector envMap;
  SpecularMap::Selector specularMap;
  Specular::Selector specular;
  DisplacementMap::Selector displacementMap;
  BumpMap::Selector bumpMap;
  NormalMap::Selector normalMap;
  RoughnessMap::Selector roughnessMap;
  MetalnessMap::Selector metalnessMap;

  Selector(const Colored::Selector &m) : colored(m) {}
  Selector(const LightMap::Selector &m) : lightMap(m) {}
  Selector(const Emissive::Selector &m) : emissive(m) {}
  Selector(const AoMap::Selector &m) : aoMap(m) {}
  Selector(const AlphaMap::Selector &m) : alphaMap(m) {}
  Selector(const EnvMap::Selector &m) : envMap(m) {}
  Selector(const SpecularMap::Selector &m) : specularMap(m) {}
  Selector(const Specular::Selector &m) : specular(m) {}
  Selector(const DisplacementMap::Selector &m) : displacementMap(m) {}
  Selector(const BumpMap::Selector &m) : bumpMap(m) {}
  Selector(const NormalMap::Selector &m) : normalMap(m) {}
  Selector(const RoughnessMap::Selector &m) : roughnessMap(m) {}
  Selector(const MetalnessMap::Selector &m) : metalnessMap(m) {}
};

}

struct DLX Material
{
  static uint16_t ___material_id_count;

  const sole::uuid uuid;
  uint16_t id;

  std::string name;

  bool fog = true;
  bool lights = true;

  Color ambientColor = Color::null();

  Blending blending = Blending::Normal;
  Side side = Side::Front;
  bool flatShading = false;
  Colors vertexColors = Colors::None;

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
  unsigned wireframeLineWidth = 1;
  LineCap wireframeLineCap = LineCap::round;
  LineJoin wireframeLineJoin = LineJoin::round;

  Precision precision = Precision::unknown; // override the renderer's default precision for this material

  bool polygonOffset = false;
  unsigned polygonOffsetFactor = 0;
  unsigned polygonOffsetUnits = 0;

  bool dithering = false;

  float alphaTest = 0;
  bool premultipliedAlpha = false;

  Texture::Ptr map;

  bool skinning = false;
  bool morphTargets = false;
  unsigned numSupportedMorphTargets = 0;
  bool morphNormals = false;
  unsigned numSupportedMorphNormals = 0;

  unsigned overdraw = 0; // Overdrawn pixels (typically between 0 and 1) for fixing antialiasing gaps in CanvasRenderer

  bool visible = true;

  bool needsUpdate = true;

  material::Info info;
  material::Typer typer;

  Signal<void(Material *)> onDispose;

protected:
  Material(const material::Info &info, const material::Typer &typer)
     : uuid(sole::uuid4()), id(___material_id_count++), info(info), typer(typer) {}

  Material(const Material &material, const material::Info &info, const material::Typer &typer);

  virtual void callback(const material::Selector &selector) {}

public:
  virtual ~Material() {}

  using Ptr = std::shared_ptr<Material>;

  template <typename Mixin>
  void mixinDo(std::function<void(Mixin &m)> func) {
    callback(material::Selector(func));
  }

  virtual void dispose() {
    onDispose.emitSignal(this);
    onDispose.disconnectAll();
  }

  template <typename Mat> bool is() {
    return (bool)((Mat *)typer);
  }

  virtual bool transparent() const {return false;}

  virtual Material *cloned() const = 0;

  virtual void setupPointLight(const math::Vector3 &position, float near, float far) {}
};

template <typename ... Maps>
struct MixinResolve;

template <typename ... Maps>
struct MaterialT;

template<typename Map>
struct MaterialT<Map> : public Material, public Map
{
  using ResolveT = MixinResolve<Map>;

protected:
  MaterialT(const material::Info &info, const material::Typer &typer) : Material(info, typer) {}
  MaterialT(const MaterialT &material, const material::Info &info, const material::Typer &typer)
     : Map(material), Material(material, info, typer) {}
};

template<>
struct MaterialT<> : public Material
{
protected:
  MaterialT(const material::Info &info, const material::Typer &typer) : Material(info, typer) {}
  MaterialT(const MaterialT &material, const material::Info &info, const material::Typer &typer)
     : Material(material, info, typer) {}
};

template<typename Map, typename ... Maps>
struct MaterialT<Map, Maps...> : public Material, Map, Maps...
{
  using ResolveT = MixinResolve<Map, Maps...>;

protected:
  MaterialT(const material::Info &info, const material::Typer &typer) : Material(info, typer) {}
  MaterialT(const MaterialT &material, const material::Info &info, const material::Typer &typer)
     : Map(material), Maps(material)..., Material(material, info, typer) {}
};

}
#endif //THREEPP_MATERIAL_H
