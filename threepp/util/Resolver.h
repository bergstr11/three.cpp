//
// Created by byter on 24.09.17.
//

#ifndef THREEPP_RESOLVER_H
#define THREEPP_RESOLVER_H

#include <memory>
#include <functional>
#include <array>
#include <string>
#include <set>

#include <threepp/renderers/gl/shader/ShaderID.h>
#include "Typer.h"

namespace resolver {

/**
 * helper template that serves as implementation container. This setup is the result of
 * googl'ing and experimentation, because declaring virtual functions inside the resolver
 * chain templates did not work out. The Callback mechanism was devised to pass control back
 * to the basetype-aware resolver chain
 *
 * @tparam Tmap
 */
template<typename Tmap>
class Resolve
{
public:
  struct Callback
  {
    virtual typename Tmap::value_type getValue(Tmap &t) const = 0;
  };

  Callback * callback = nullptr;

  typename Tmap::value_type getValue(Tmap &t) {
    if(callback) return callback->getValue(t);
    return Tmap::getNull();
  }
};

/**
 * base template for the basetype-agnostic resolver chain. Resolvers is a variadic template
 * which recursively builds a class hierarchy which implements the basetype-agnostic interface
 * for a collection of type maps. A type map holds associations between type (aka classes) and
 * values of a given type.
 *
 * @tparam Tmaps a list of type maps
 */
template<typename ...Tmaps>
class Resolvers;

/** variadic template terminator */
template<>
class Resolvers<>
{
};

/**
 * template specialization for the for the basetype-agnostic resolver chain.
 * Main contribution is the implementation of the callback invoker functions
 *
 * @tparam Tmap a type map
 * @tparam Tmaps more type maps
 */
template<typename Tmap, typename ...Tmaps>
class Resolvers<Tmap, Tmaps...> : public Resolvers<Tmaps...>, public virtual Resolve<Tmap>
{
public:
  using Ptr = std::shared_ptr<Resolvers>;
  static Ptr makeNull() {return Ptr(new Resolvers());}
};

static const nullptr_t null = nullptr;

template <typename T, typename V>
class Assoc
{
  V v;

public:
  Assoc() {}
  Assoc(const V v) : v(v) {}
  Assoc(const Assoc &other) : v(other.v) {}

  const V &operator()(T &t) const {
    return v;
  }
};

template <typename T>
using StringAssoc = Assoc<T, std::string>;

/**
 * base template for the basetype-aware resolver chain. The basetype-aware resolver chain inherits
 * from the basetype-agnostic chain, thus making polymorphic deployment possible
 *
 * @tparam Base the base type for this resolver chain
 * @tparam Tmaps the type maps
 */
template<typename Base, typename ...Tmaps>
class ResolversT;

/** variadic template terminator */
template<typename Base>
class ResolversT<Base>
{
protected:
  Base &b;

  ResolversT(Base &b) : b(b) {}
};

/**
 * template specialization for the basetype-aware resolver chain
 *
 * @tparam Base base type
 * @tparam Tmap the current type map
 * @tparam Tmaps the remaining type maps
 */
template<typename Base, typename Tmap, typename ...Tmaps>
class ResolversT<Base, Tmap, Tmaps...> : public ResolversT<Base, Tmaps...>, public Resolvers<Tmap, Tmaps...>, public Resolve<Tmap>::Callback
{
  using Super = ResolversT<Base, Tmaps...>;

public:
  ResolversT(Base &b) : Super(b) {
    Resolve<Tmap>::callback = this;
  }

  typename Tmap::value_type getValue(Tmap &t) const override {
    return t.value(this->b);
  }

  using Ptr = std::shared_ptr<ResolversT>;
  static Ptr make(Base &b) {return Ptr(new ResolversT(b));}
};

} //namespace: resolver

#define DEF_VALUETABLE(Cls, Vtype, Dflt) \
struct Cls { \
using value_type = Vtype; \
static value_type getNull() {throw std::logic_error("callback not set");} \
template <typename T> Vtype value(T &t) const {return Dflt;}\
}; \
using Cls##Resolver = resolver::Resolve<Cls>;


#define PUT_VALUETABLE(Cls, Mtype, Vtype, Val) \
template <> inline Vtype Cls::value(Mtype &t) const { \
static const resolver::Assoc<Mtype, Vtype> sa {Val}; \
return sa(t); \
}

#define DEF_STRINGTABLE(Cls, Dflt) \
struct Cls { \
using value_type = const char *; \
static value_type getNull() {throw std::logic_error("callback not set");} \
template <typename T> const char *value(T &t) const {return Dflt;} \
}; \
using Cls##Resolver = resolver::Resolve<Cls>;

#define PUT_STRINGTABLE(Cls, Type, Val) \
template <> inline const  char *Cls::value(Type &t) const { \
static const resolver::Assoc<Type, const char *> sa {Val}; \
return sa(t); \
}

#define DEF_RESOLVER_1(Map) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map>; \
using Resolver = resolver::Resolvers<Map>;

#define DEF_RESOLVER_2(Map1, Map2) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map1, Map2>; \
using Resolver = resolver::Resolvers<Map1, Map2>;

#define DEF_RESOLVER_3(Map1, Map2, Map3) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map1, Map2, Map3>; \
using Resolver = resolver::Resolvers<Map1, Map2, Map3>;

#define DEF_RESOLVER_4(Map1, Map2, Map3, Map4) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map1, Map2, Map3, Map4>; \
using Resolver = resolver::Resolvers<Map1, Map2, Map3, Map4>;

namespace three {

class ImageCubeTexture;
class ImageTexture;
class DataTexture;
class DataCubeTexture;
class DepthTexture;
class Texture;

namespace texture {
using Typer = three::Typer<ImageCubeTexture,
   DepthTexture, ImageTexture, ImageCubeTexture, DataTexture, DataCubeTexture>;
}

class Color;

template <typename T>
class Background;

namespace background {
using Typer = three::Typer<Background<std::shared_ptr<Texture>>, Background<Color>>;
}

class Camera;
class ArrayCamera;
class OrthographicCamera;
class PerspectiveCamera;
class Light;
class AmbientLight;
class DirectionalLight;
class HemisphereLight;
class PointLight;
class RectAreaLight;
class SpotLight;
class TargetLight;
class Line;
class LineSegments;
class Points;
class Mesh;
class SkinnedMesh;
class Sprite;
class ImmediateRenderObject;
class LensFlare;

namespace object {
using Typer = three::Typer<Camera, ArrayCamera, OrthographicCamera, PerspectiveCamera,
   Light, AmbientLight, DirectionalLight, HemisphereLight, PointLight, RectAreaLight, SpotLight, TargetLight,
   Line, LineSegments, Mesh, Sprite, ImmediateRenderObject, Points, SkinnedMesh, LensFlare>;
}

class LinearGeometry;
class BufferGeometry;
class InstancedBufferGeometry;
class DirectGeometry;

namespace geometry {
using Typer = three::Typer<LinearGeometry, BufferGeometry, InstancedBufferGeometry, DirectGeometry>;
}

class Material;
class ShaderMaterial;
class RawShaderMaterial;
class LineBasicMaterial;
class LineDashedMaterial;
class MeshPhongMaterial;
class MeshToonMaterial;
class MeshDepthMaterial;
class MeshNormalMaterial;
class MeshStandardMaterial;
class MeshPhysicalMaterial;
class MeshDistanceMaterial;
class MeshLambertMaterial;
class MeshBasicMaterial;
class PointsMaterial;
class ShadowMaterial;
class SpriteMaterial;

namespace material {

using Typer = three::Typer<Material, ShaderMaterial, RawShaderMaterial, LineBasicMaterial,
   LineDashedMaterial, MeshPhongMaterial, MeshToonMaterial, MeshDepthMaterial, MeshNormalMaterial,
   MeshStandardMaterial, MeshPhysicalMaterial, MeshDistanceMaterial, MeshLambertMaterial,
   MeshBasicMaterial, PointsMaterial, ShadowMaterial, SpriteMaterial>;


DEF_VALUETABLE(ShaderIDs, gl::ShaderID, gl::ShaderID::undefined)
PUT_VALUETABLE(ShaderIDs, MeshDepthMaterial, gl::ShaderID, gl::ShaderID::depth)
PUT_VALUETABLE(ShaderIDs, MeshDistanceMaterial, gl::ShaderID, gl::ShaderID::distanceRGBA)
PUT_VALUETABLE(ShaderIDs, MeshNormalMaterial, gl::ShaderID, gl::ShaderID::normal)
PUT_VALUETABLE(ShaderIDs, MeshBasicMaterial, gl::ShaderID, gl::ShaderID::basic)
PUT_VALUETABLE(ShaderIDs, MeshLambertMaterial, gl::ShaderID, gl::ShaderID::lambert)
PUT_VALUETABLE(ShaderIDs, MeshPhongMaterial, gl::ShaderID, gl::ShaderID::phong)
PUT_VALUETABLE(ShaderIDs, MeshToonMaterial, gl::ShaderID, gl::ShaderID::phong)
PUT_VALUETABLE(ShaderIDs, MeshStandardMaterial, gl::ShaderID, gl::ShaderID::standard)
PUT_VALUETABLE(ShaderIDs, MeshPhysicalMaterial, gl::ShaderID, gl::ShaderID::physical)
PUT_VALUETABLE(ShaderIDs, LineBasicMaterial, gl::ShaderID, gl::ShaderID::basic)
PUT_VALUETABLE(ShaderIDs, LineDashedMaterial, gl::ShaderID, gl::ShaderID::dashed)
PUT_VALUETABLE(ShaderIDs, PointsMaterial, gl::ShaderID, gl::ShaderID::points)
PUT_VALUETABLE(ShaderIDs, ShadowMaterial, gl::ShaderID, gl::ShaderID::shadow)

DEF_STRINGTABLE(ShaderNames, "Material")
PUT_STRINGTABLE(ShaderNames, MeshDepthMaterial, "MeshDepthMaterial")
PUT_STRINGTABLE(ShaderNames, MeshDistanceMaterial, "MeshDistanceMaterial")
PUT_STRINGTABLE(ShaderNames, MeshNormalMaterial, "MeshNormalMaterial")
PUT_STRINGTABLE(ShaderNames, MeshBasicMaterial, "MeshBasicMaterial")
PUT_STRINGTABLE(ShaderNames, MeshLambertMaterial, "MeshLambertMaterial")
PUT_STRINGTABLE(ShaderNames, MeshPhongMaterial, "MeshPhongMaterial")
PUT_STRINGTABLE(ShaderNames, MeshToonMaterial, "MeshToonMaterial")
PUT_STRINGTABLE(ShaderNames, MeshStandardMaterial, "MeshStandardMaterial")
PUT_STRINGTABLE(ShaderNames, MeshPhysicalMaterial, "MeshPhysicalMaterial")
PUT_STRINGTABLE(ShaderNames, LineBasicMaterial, "LineBasicMaterial")
PUT_STRINGTABLE(ShaderNames, LineDashedMaterial, "LineDashedMaterial")
PUT_STRINGTABLE(ShaderNames, PointsMaterial, "PointsMaterial")
PUT_STRINGTABLE(ShaderNames, ShaderMaterial, "ShaderMaterial")
PUT_STRINGTABLE(ShaderNames, RawShaderMaterial, "RawShaderMaterial")
PUT_STRINGTABLE(ShaderNames, ShadowMaterial, "ShadowMaterial")
PUT_STRINGTABLE(ShaderNames, SpriteMaterial, "SpriteMaterial")

DEF_RESOLVER_2(ShaderIDs, ShaderNames)

}

#define CAST(x, y, Cls) auto y = std::dynamic_pointer_cast<Cls>(x)
#define CAST2(x, Cls) std::dynamic_pointer_cast<Cls>(x)
}

#endif //THREEPP_RESOLVER_H
