//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_RESOLVER_H
#define THREE_QT_RESOLVER_H

#if 0
#include <light/AmbientLight.h>
#include <light/DirectionalLight.h>
#include <light/HemisphereLight.h>
#include <light/PointLight.h>
#include <light/RectAreaLight.h>
#include <light/SpotLight.h>
#endif

#include <memory>
#include <functional>

#define DEF_RESOLVER() \
struct ResolverBase  { \
  using Ptr = std::shared_ptr<ResolverBase>; \
  virtual bool call(Functions &functions) const {} \
}; \
template <typename L> \
class Resolver {};

#define MK_RESOLVER(Clz, func) \
template <> \
struct Resolver<Clz> : public ResolverBase \
{ \
  Clz &target; \
  explicit Resolver(Clz &target) : target(target) {} \
  static Ptr make(Clz &target) {return Ptr(new Resolver<Clz>(target));} \
  bool call(Functions &f) const override {if(f.func) {f.func(target); return true;} return false;} \
};

#define MK_VOID_RESOLVER(func) \
template <> \
struct Resolver<void> : public ResolverBase \
{ \
  static Ptr make() {return Ptr(new Resolver<void>());} \
  bool call(Functions &f) const override {if(f.func) {f.func(); return true;} return false;} \
};

#define MK_BOOL_RESOLVER() \
template <> \
struct Resolver<bool> : public ResolverBase \
{ \
  bool what; \
  explicit Resolver(bool what) : what(what) {} \
  static Ptr make(bool what) {return Ptr(new Resolver<bool>(what));} \
  bool call(Functions &f) const override {return what;} \
};

namespace resolver {

template <typename L>
using Func = std::function<void(L &)>;

using Void = std::function<void()>;

}

namespace three {

class AmbientLight;
class DirectionalLight;
class HemisphereLight;
class PointLight;
class RectAreaLight;
class SpotLight;

namespace light {

struct Functions
{
  resolver::Func<AmbientLight> ambient;
  resolver::Func<DirectionalLight> directional;
  resolver::Func<HemisphereLight> hemisphere;
  resolver::Func<PointLight> point;
  resolver::Func<RectAreaLight> rectarea;
  resolver::Func<SpotLight> spot;
};

DEF_RESOLVER()

MK_RESOLVER(AmbientLight, ambient)
MK_RESOLVER(DirectionalLight, directional)
MK_RESOLVER(HemisphereLight, hemisphere)
MK_RESOLVER(PointLight, point)
MK_RESOLVER(RectAreaLight, rectarea)
MK_RESOLVER(SpotLight, spot)

}

class CubeTexture;
class Texture;
class Color;

namespace scene {

struct Functions
{
  resolver::Func<std::shared_ptr<CubeTexture>> cubeTexture;
  resolver::Func<std::shared_ptr<Texture>> texture;
  resolver::Func<Color> color;
  resolver::Void _void;
};

DEF_RESOLVER()

MK_RESOLVER(std::shared_ptr<CubeTexture>, cubeTexture)
MK_RESOLVER(std::shared_ptr<Texture>, texture)
MK_RESOLVER(Color, color)
MK_VOID_RESOLVER(_void)

}

class Light;
class Sprite;
class LensFlare;
class Mesh;
class SkinnedMesh;
class Line;
class Points;

namespace object {

struct Functions
{
  resolver::Func<Light> light;
  resolver::Func<Sprite> sprite;
  resolver::Func<LensFlare> lensFlare;
  resolver::Func<Mesh> mesh;
  resolver::Func<SkinnedMesh> skinnedMesh;
  resolver::Func<Line> line;
  resolver::Func<Points> points;
  resolver::Void _void;
};

DEF_RESOLVER()

MK_RESOLVER(Light, light)
MK_RESOLVER(Sprite, sprite)
MK_RESOLVER(LensFlare, lensFlare)
MK_RESOLVER(Mesh, mesh)
MK_RESOLVER(Line, line)
MK_RESOLVER(Points, points)
MK_VOID_RESOLVER(_void)

}

class InterleavedBufferAttribute;

namespace bufferattribute {

struct Functions
{
  resolver::Func<InterleavedBufferAttribute> interleaved;
  resolver::Void _void;
};

DEF_RESOLVER()

MK_RESOLVER(InterleavedBufferAttribute, interleaved)
MK_VOID_RESOLVER(_void)

}

class DefaultFog;
class FogExp2;

namespace fog {

struct Functions
{
  resolver::Func<DefaultFog> fogDefault;
  resolver::Func<FogExp2> fogExp2;
};

DEF_RESOLVER()

MK_RESOLVER(DefaultFog, fogDefault)
MK_RESOLVER(FogExp2, fogExp2)

}

class Camera;
class ArrayCamera;
class PerspectiveCamera;

namespace camera {

struct Functions
{
  resolver::Func<ArrayCamera> array;
  resolver::Func<PerspectiveCamera> perspective;
  resolver::Func<Camera> base;
};

DEF_RESOLVER()

MK_RESOLVER(PerspectiveCamera, perspective)
MK_RESOLVER(ArrayCamera, array)
MK_RESOLVER(Camera, base)

}

class Material;
class ShaderMaterial;
class MeshPhongMaterial;
class MeshDepthMaterial;
class MeshStandardMaterial;
class MeshDistanceMaterial;
class MeshLambertMaterial;
class MeshBasicMaterial;
class SpriteMaterial;

namespace material {

struct Functions
{
  resolver::Func<ShaderMaterial> shader;
  resolver::Func<MeshPhongMaterial> meshPhong;
  resolver::Func<MeshDepthMaterial> meshDepth;
  resolver::Func<MeshDistanceMaterial> meshDistance;
  resolver::Func<MeshStandardMaterial> meshStandard;
  resolver::Func<MeshLambertMaterial> meshLambert;
  resolver::Func<MeshBasicMaterial> meshBasic;
  resolver::Func<SpriteMaterial> sprite;
};

DEF_RESOLVER()

MK_RESOLVER(ShaderMaterial, shader);
MK_RESOLVER(MeshPhongMaterial, meshPhong);
MK_RESOLVER(MeshDepthMaterial, meshDepth);
MK_RESOLVER(MeshDistanceMaterial, meshDistance);
MK_RESOLVER(MeshStandardMaterial, meshStandard);
MK_RESOLVER(MeshLambertMaterial, meshLambert);
MK_RESOLVER(MeshBasicMaterial, meshBasic);
MK_RESOLVER(SpriteMaterial, sprite);
MK_BOOL_RESOLVER()

}
}
#endif //THREE_QT_RESOLVER_H
