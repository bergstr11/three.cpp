//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_RESOLVER_H
#define THREE_QT_RESOLVER_H

#include <memory>
#include <functional>
#include <array>
#include <string>
#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <stdexcept>

#include <renderers/gl/shader/ShaderID.h>

namespace resolver {

static const nullptr_t null = nullptr;

template<typename T>
struct Functor
{
  virtual void operator()(T &t) const = 0;
  virtual ~Functor() {}
};

template<typename T, typename F>
struct FunctorT : public Functor<T>
{
  const F &f;

  FunctorT(const F &f) : f(f) {}

  void operator()(T &s) const override
  {
    f(s);
  }
};

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

template<typename T>
class Assoc<T, Functor<T>>
{
  Functor<T> * ft = nullptr;

public:
  template <typename F>
  Assoc(F&& f) : ft(new FunctorT<T, F>(f)) {}
  Assoc() : ft(nullptr) {}
  Assoc(Assoc &&ass) : ft(ass.ft) {
    ass.ft = nullptr;
  }

  template <typename F>
  void operator =(F&& f) {
    ft = new FunctorT<T, F>(f);
  }

  bool operator()(T &t) const {
    if(ft) {(*ft)(t); return true;}
    return false;
  }

  void clear() {
    if(ft) {
      delete ft;
      ft = nullptr;
    }
  }

  ~Assoc() {
    if(ft) delete ft;
  }
};

struct NullFuncAssoc {
  template <typename T> bool operator()(T &t) {}
};
template <typename T>
using FuncAssoc = Assoc<T, Functor<T>>;
template <typename T>
using StringAssoc = Assoc<T, std::string>;

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
    virtual typename Tmap::value_type getValue(const Tmap &t) const = 0;
  };

  Callback *callback = nullptr;

  typename Tmap::value_type getValue(const Tmap &t) {
    if(callback) return callback->getValue(t);
    throw std::logic_error("callback not set");
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
};


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

  typename Tmap::value_type getValue(const Tmap &t) const override {
    return t.value(this->b);
  }

  using Ptr = std::shared_ptr<ResolversT>;
  static Ptr make(Base &b) {return Ptr(new ResolversT(b));}
  static Ptr make() {return Ptr(resolver::null);}
};

} //namespace: resolver

#define DEF_VALUE_TABLE(Cls, Vtype, Dflt) \
struct Cls { \
using value_type = Vtype; \
template <typename T> Vtype value(T &t) const {return Dflt;}\
}; \
using Cls##Resolver = resolver::Resolve<Cls>;


#define PUT_VALUE_TABLE(Cls, Mtype, Vtype, Val) \
template <> inline Vtype Cls::value(Mtype &t) const { \
static const resolver::Assoc<Mtype, Vtype> sa {Val}; \
return sa(t); \
}

#define DEF_STRING_TABLE(Cls, Dflt) \
struct Cls { \
using value_type = const char *; \
template <typename T> const char *value(T &t) const {return Dflt;} \
}; \
using Cls##Resolver = resolver::Resolve<Cls>;


#define PUT_STRING_TABLE(Cls, Type, Val) \
template <> inline const  char *Cls::value(Type &t) const { \
static const resolver::Assoc<Type, const char *> sa {#Val}; \
return sa(t); \
}

#define DEF_FUNC_TABLE(Cls) \
struct Cls { \
using value_type = bool; \
template <typename T> resolver::FuncAssoc<T> &func() const = delete; \
template <typename T> bool value(T &t) const = delete; \
}; \
using Cls##Resolver = resolver::Resolve<Cls>;

#define PUT_FUNC_TABLE(Cls, Type) \
template <> inline resolver::FuncAssoc<Type> &Cls::func() const { \
thread_local static resolver::FuncAssoc<Type> f; \
return f; \
} \
template <> inline bool Cls::value(Type &t) const {return func<Type>()(t);}

#define DEF_RESOLVER_1(Map) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map>; \
using NullResolver = resolver::ResolversT<nullptr_t, Map>; \
using Resolver = resolver::Resolvers<Map>;

#define DEF_RESOLVER_2(Map1, Map2) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map1, Map2>; \
using NullResolver = resolver::ResolversT<nullptr_t, Map1, Map2>; \
using Resolver = resolver::Resolvers<Map1, Map2>;

#define DEF_RESOLVER_3(Map1, Map2, Map3) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map1, Map2, Map3>; \
using NullResolver = resolver::ResolversT<nullptr_t, Map1, Map2, Map3>; \
using Resolver = resolver::Resolvers<Map1, Map2, Map3>;

#define DEF_RESOLVER_4(Map1, Map2, Map3, Map4) \
template <typename Obj> \
using ResolverT = resolver::ResolversT<Obj, Map1, Map2, Map3, Map4>; \
using NullResolver = resolver::ResolversT<nullptr_t, Map1, Map2, Map3, Map4>; \
using Resolver = resolver::Resolvers<Map1, Map2, Map3, Map4>;

namespace three {

class AmbientLight;
class DirectionalLight;
class HemisphereLight;
class PointLight;
class RectAreaLight;
class SpotLight;

namespace light {

DEF_FUNC_TABLE(Dispatch)
PUT_FUNC_TABLE(Dispatch, AmbientLight)
PUT_FUNC_TABLE(Dispatch, DirectionalLight)
PUT_FUNC_TABLE(Dispatch, HemisphereLight)
PUT_FUNC_TABLE(Dispatch, PointLight)
PUT_FUNC_TABLE(Dispatch, RectAreaLight)
PUT_FUNC_TABLE(Dispatch, SpotLight)

DEF_RESOLVER_1(Dispatch)

}

class CubeTexture;
class DefaultTexture;
class Color;

namespace scene {

DEF_FUNC_TABLE(BackgroundDispatch)
PUT_FUNC_TABLE(BackgroundDispatch, nullptr_t)
PUT_FUNC_TABLE(BackgroundDispatch, std::shared_ptr<CubeTexture>)
PUT_FUNC_TABLE(BackgroundDispatch, std::shared_ptr<DefaultTexture>)
PUT_FUNC_TABLE(BackgroundDispatch, Color)

DEF_RESOLVER_1(BackgroundDispatch)

}

class Texture;
class CompressedTexture;
class DataTexture;
class DepthTexture;

namespace texture {

DEF_FUNC_TABLE(Dispatch)
PUT_FUNC_TABLE(Dispatch, Texture)
PUT_FUNC_TABLE(Dispatch, DefaultTexture)
PUT_FUNC_TABLE(Dispatch, CubeTexture)
PUT_FUNC_TABLE(Dispatch, CompressedTexture)
PUT_FUNC_TABLE(Dispatch, DataTexture)
PUT_FUNC_TABLE(Dispatch, DepthTexture)

DEF_RESOLVER_1(Dispatch)
}

class Light;
class Sprite;
class LensFlare;
class Mesh;
class SkinnedMesh;
class Line;
class Points;
class ImmediateRenderObject;

namespace object {

DEF_FUNC_TABLE(Dispatch)
PUT_FUNC_TABLE(Dispatch, nullptr_t)
PUT_FUNC_TABLE(Dispatch, Light)
PUT_FUNC_TABLE(Dispatch, Sprite)
PUT_FUNC_TABLE(Dispatch, LensFlare)
PUT_FUNC_TABLE(Dispatch, Mesh)
PUT_FUNC_TABLE(Dispatch, SkinnedMesh)
PUT_FUNC_TABLE(Dispatch, Line)
PUT_FUNC_TABLE(Dispatch, Points)
PUT_FUNC_TABLE(Dispatch, ImmediateRenderObject)

DEF_RESOLVER_1(Dispatch)

}

class InterleavedBufferAttribute;

namespace bufferattribute {

DEF_FUNC_TABLE(Dispatch)
PUT_FUNC_TABLE(Dispatch, nullptr_t)
PUT_FUNC_TABLE(Dispatch, InterleavedBufferAttribute)

DEF_RESOLVER_1(Dispatch)

}

class DefaultFog;
class FogExp2;

namespace fog {

DEF_FUNC_TABLE(Dispatch)
PUT_FUNC_TABLE(Dispatch, DefaultFog)
PUT_FUNC_TABLE(Dispatch, FogExp2)

DEF_RESOLVER_1(Dispatch)

}

class Camera;
class ArrayCamera;
class PerspectiveCamera;

namespace camera {

DEF_FUNC_TABLE(Dispatch)
PUT_FUNC_TABLE(Dispatch, Camera)
PUT_FUNC_TABLE(Dispatch, ArrayCamera)
PUT_FUNC_TABLE(Dispatch, PerspectiveCamera)

DEF_RESOLVER_1(Dispatch)

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

DEF_FUNC_TABLE(Dispatch)
PUT_FUNC_TABLE(Dispatch, Material)
PUT_FUNC_TABLE(Dispatch, ShaderMaterial)
PUT_FUNC_TABLE(Dispatch, RawShaderMaterial)
PUT_FUNC_TABLE(Dispatch, LineBasicMaterial)
PUT_FUNC_TABLE(Dispatch, LineDashedMaterial)
PUT_FUNC_TABLE(Dispatch, MeshPhongMaterial)
PUT_FUNC_TABLE(Dispatch, MeshDepthMaterial)
PUT_FUNC_TABLE(Dispatch, MeshDistanceMaterial)
PUT_FUNC_TABLE(Dispatch, MeshStandardMaterial)
PUT_FUNC_TABLE(Dispatch, MeshLambertMaterial)
PUT_FUNC_TABLE(Dispatch, MeshBasicMaterial)
PUT_FUNC_TABLE(Dispatch, MeshNormalMaterial)
PUT_FUNC_TABLE(Dispatch, MeshPhysicalMaterial)
PUT_FUNC_TABLE(Dispatch, MeshToonMaterial)
PUT_FUNC_TABLE(Dispatch, PointsMaterial)
PUT_FUNC_TABLE(Dispatch, SpriteMaterial)

DEF_VALUE_TABLE(ShaderIDs, gl::ShaderID, gl::ShaderID::undefined)
PUT_VALUE_TABLE(ShaderIDs, MeshDepthMaterial, gl::ShaderID, gl::ShaderID::depth)
PUT_VALUE_TABLE(ShaderIDs, MeshDistanceMaterial, gl::ShaderID, gl::ShaderID::distanceRGBA)
PUT_VALUE_TABLE(ShaderIDs, MeshNormalMaterial, gl::ShaderID, gl::ShaderID::normal)
PUT_VALUE_TABLE(ShaderIDs, MeshBasicMaterial, gl::ShaderID, gl::ShaderID::basic)
PUT_VALUE_TABLE(ShaderIDs, MeshLambertMaterial, gl::ShaderID, gl::ShaderID::lambert)
PUT_VALUE_TABLE(ShaderIDs, MeshPhongMaterial, gl::ShaderID, gl::ShaderID::phong)
PUT_VALUE_TABLE(ShaderIDs, MeshToonMaterial, gl::ShaderID, gl::ShaderID::phong)
PUT_VALUE_TABLE(ShaderIDs, MeshStandardMaterial, gl::ShaderID, gl::ShaderID::physical)
PUT_VALUE_TABLE(ShaderIDs, MeshPhysicalMaterial, gl::ShaderID, gl::ShaderID::physical)
PUT_VALUE_TABLE(ShaderIDs, LineBasicMaterial, gl::ShaderID, gl::ShaderID::basic)
PUT_VALUE_TABLE(ShaderIDs, LineDashedMaterial, gl::ShaderID, gl::ShaderID::dashed)
PUT_VALUE_TABLE(ShaderIDs, PointsMaterial, gl::ShaderID, gl::ShaderID::points)
PUT_VALUE_TABLE(ShaderIDs, ShadowMaterial, gl::ShaderID, gl::ShaderID::shadow)

DEF_STRING_TABLE(ShaderNames, "Material")
PUT_STRING_TABLE(ShaderNames, MeshDepthMaterial, "")
PUT_STRING_TABLE(ShaderNames, MeshDistanceMaterial, "MeshDepthMaterial")
PUT_STRING_TABLE(ShaderNames, MeshNormalMaterial, "MeshNormalMaterial")
PUT_STRING_TABLE(ShaderNames, MeshBasicMaterial, "MeshBasicMaterial")
PUT_STRING_TABLE(ShaderNames, MeshLambertMaterial, "MeshLambertMaterial")
PUT_STRING_TABLE(ShaderNames, MeshPhongMaterial, "MeshPhongMaterial")
PUT_STRING_TABLE(ShaderNames, MeshToonMaterial, "MeshToonMaterial")
PUT_STRING_TABLE(ShaderNames, MeshStandardMaterial, "MeshStandardMaterial")
PUT_STRING_TABLE(ShaderNames, MeshPhysicalMaterial, "MeshPhysicalMaterial")
PUT_STRING_TABLE(ShaderNames, LineBasicMaterial, "LineBasicMaterial")
PUT_STRING_TABLE(ShaderNames, LineDashedMaterial, "LineDashedMaterial")
PUT_STRING_TABLE(ShaderNames, PointsMaterial, "PointsMaterial")
PUT_STRING_TABLE(ShaderNames, ShaderMaterial, "ShaderMaterial")
PUT_STRING_TABLE(ShaderNames, RawShaderMaterial, "RawShaderMaterial")
PUT_STRING_TABLE(ShaderNames, ShadowMaterial, "ShadowMaterial")
PUT_STRING_TABLE(ShaderNames, SpriteMaterial, "SpriteMaterial")

DEF_RESOLVER_3(Dispatch, ShaderIDs, ShaderNames)

}

}

#endif //THREE_QT_RESOLVER_H
