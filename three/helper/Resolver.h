//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_RESOLVER_H
#define THREE_QT_RESOLVER_H

#include <memory>
#include <functional>
#include <array>
#include <string>
#include <set>

#include <renderers/gl/shader/ShaderID.h>

namespace resolver {

static const nullptr_t null = nullptr;

struct Clearable
{
  virtual void clear() = 0;
};

class Dispatcher {
protected:
  std::set<Clearable *> _clearables;

public:
  void clear() {
    for(auto clearable : _clearables) clearable->clear();
    _clearables.clear();
  }

protected:
  ~Dispatcher() {
    clear();
  }
};

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
class Assoc<T, Functor<T>> : public Clearable
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
    clear();
    ft = new FunctorT<T, F>(f);
  }

  bool operator()(T &t) {
    if(ft) {
      (*ft)(t);
      return true;
    }
    return false;
  }

  void clear() override {
    if(ft) {
      delete ft;
      ft = nullptr;
    }
  }

  ~Assoc() {
    clear();
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

#define DEF_FUNCTABLE(Cls) \
struct Cls : public resolver::Dispatcher { \
using value_type = bool; \
static value_type getNull() {return false;} \
template <typename T> resolver::FuncAssoc<T> &func() = delete; \
template <typename T> bool value(T &t) = delete; \
}; \
using Cls##Resolver = resolver::Resolve<Cls>;

#define PUT_FUNCTABLE(Cls, Type) \
template <> inline resolver::FuncAssoc<Type> &Cls::func() { \
thread_local static resolver::FuncAssoc<Type> f; \
_clearables.insert(&f); \
return f; \
} \
template <> inline bool Cls::value(Type &t) {return func<Type>()(t);}

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

class AmbientLight;
class DirectionalLight;
class HemisphereLight;
class PointLight;
class RectAreaLight;
class SpotLight;

namespace light {

DEF_FUNCTABLE(Dispatch)
PUT_FUNCTABLE(Dispatch, AmbientLight)
PUT_FUNCTABLE(Dispatch, DirectionalLight)
PUT_FUNCTABLE(Dispatch, HemisphereLight)
PUT_FUNCTABLE(Dispatch, PointLight)
PUT_FUNCTABLE(Dispatch, RectAreaLight)
PUT_FUNCTABLE(Dispatch, SpotLight)

DEF_RESOLVER_1(Dispatch)

}

class ImageCubeTexture;
class ImageTexture;
class Color;

namespace scene {

DEF_FUNCTABLE(BackgroundDispatch)
PUT_FUNCTABLE(BackgroundDispatch, std::shared_ptr<ImageCubeTexture>)
PUT_FUNCTABLE(BackgroundDispatch, std::shared_ptr<ImageTexture>)
PUT_FUNCTABLE(BackgroundDispatch, Color)

DEF_RESOLVER_1(BackgroundDispatch)

}

class Texture;
class ImageTexture;
class ImageCubeTexture;
class DataTexture;
class DataCubeTexture;
class DepthTexture;

namespace texture {

DEF_FUNCTABLE(Dispatch)
PUT_FUNCTABLE(Dispatch, Texture)
PUT_FUNCTABLE(Dispatch, ImageTexture)
PUT_FUNCTABLE(Dispatch, ImageCubeTexture)
PUT_FUNCTABLE(Dispatch, DataTexture)
PUT_FUNCTABLE(Dispatch, DataCubeTexture)
PUT_FUNCTABLE(Dispatch, DepthTexture)

DEF_RESOLVER_1(Dispatch)
}

class Light;
class Sprite;
class LensFlare;
class Mesh;
class SkinnedMesh;
class Line;
class LineSegments;
class Points;
class ImmediateRenderObject;

namespace object {

DEF_FUNCTABLE(Dispatch)
PUT_FUNCTABLE(Dispatch, Light)
PUT_FUNCTABLE(Dispatch, Sprite)
PUT_FUNCTABLE(Dispatch, LensFlare)
PUT_FUNCTABLE(Dispatch, Mesh)
PUT_FUNCTABLE(Dispatch, SkinnedMesh)
PUT_FUNCTABLE(Dispatch, Line)
PUT_FUNCTABLE(Dispatch, LineSegments)
PUT_FUNCTABLE(Dispatch, Points)
PUT_FUNCTABLE(Dispatch, ImmediateRenderObject)

DEF_RESOLVER_1(Dispatch)

}

class InterleavedBufferAttribute;

namespace bufferattribute {

DEF_FUNCTABLE(Dispatch)
PUT_FUNCTABLE(Dispatch, InterleavedBufferAttribute)

DEF_RESOLVER_1(Dispatch)

}

class DefaultFog;
class FogExp2;

namespace fog {

DEF_FUNCTABLE(Dispatch)
PUT_FUNCTABLE(Dispatch, DefaultFog)
PUT_FUNCTABLE(Dispatch, FogExp2)

DEF_RESOLVER_1(Dispatch)

}

class Camera;
class ArrayCamera;
class PerspectiveCamera;

namespace camera {

DEF_FUNCTABLE(Dispatch)
PUT_FUNCTABLE(Dispatch, ArrayCamera)
PUT_FUNCTABLE(Dispatch, PerspectiveCamera)

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

DEF_FUNCTABLE(Dispatch)
PUT_FUNCTABLE(Dispatch, Material)
PUT_FUNCTABLE(Dispatch, ShaderMaterial)
PUT_FUNCTABLE(Dispatch, RawShaderMaterial)
PUT_FUNCTABLE(Dispatch, LineBasicMaterial)
PUT_FUNCTABLE(Dispatch, LineDashedMaterial)
PUT_FUNCTABLE(Dispatch, MeshPhongMaterial)
PUT_FUNCTABLE(Dispatch, MeshDepthMaterial)
PUT_FUNCTABLE(Dispatch, MeshDistanceMaterial)
PUT_FUNCTABLE(Dispatch, MeshStandardMaterial)
PUT_FUNCTABLE(Dispatch, MeshLambertMaterial)
PUT_FUNCTABLE(Dispatch, MeshBasicMaterial)
PUT_FUNCTABLE(Dispatch, MeshNormalMaterial)
PUT_FUNCTABLE(Dispatch, MeshPhysicalMaterial)
PUT_FUNCTABLE(Dispatch, MeshToonMaterial)
PUT_FUNCTABLE(Dispatch, PointsMaterial)
PUT_FUNCTABLE(Dispatch, SpriteMaterial)

DEF_VALUETABLE(ShaderIDs, gl::ShaderID, gl::ShaderID::undefined)
PUT_VALUETABLE(ShaderIDs, MeshDepthMaterial, gl::ShaderID, gl::ShaderID::depth)
PUT_VALUETABLE(ShaderIDs, MeshDistanceMaterial, gl::ShaderID, gl::ShaderID::distanceRGBA)
PUT_VALUETABLE(ShaderIDs, MeshNormalMaterial, gl::ShaderID, gl::ShaderID::normal)
PUT_VALUETABLE(ShaderIDs, MeshBasicMaterial, gl::ShaderID, gl::ShaderID::basic)
PUT_VALUETABLE(ShaderIDs, MeshLambertMaterial, gl::ShaderID, gl::ShaderID::lambert)
PUT_VALUETABLE(ShaderIDs, MeshPhongMaterial, gl::ShaderID, gl::ShaderID::phong)
PUT_VALUETABLE(ShaderIDs, MeshToonMaterial, gl::ShaderID, gl::ShaderID::phong)
PUT_VALUETABLE(ShaderIDs, MeshStandardMaterial, gl::ShaderID, gl::ShaderID::physical)
PUT_VALUETABLE(ShaderIDs, MeshPhysicalMaterial, gl::ShaderID, gl::ShaderID::physical)
PUT_VALUETABLE(ShaderIDs, LineBasicMaterial, gl::ShaderID, gl::ShaderID::basic)
PUT_VALUETABLE(ShaderIDs, LineDashedMaterial, gl::ShaderID, gl::ShaderID::dashed)
PUT_VALUETABLE(ShaderIDs, PointsMaterial, gl::ShaderID, gl::ShaderID::points)
PUT_VALUETABLE(ShaderIDs, ShadowMaterial, gl::ShaderID, gl::ShaderID::shadow)

DEF_STRINGTABLE(ShaderNames, "Material")
PUT_STRINGTABLE(ShaderNames, MeshDepthMaterial, "")
PUT_STRINGTABLE(ShaderNames, MeshDistanceMaterial, "MeshDepthMaterial")
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

DEF_RESOLVER_3(Dispatch, ShaderIDs, ShaderNames)

}

}

#endif //THREE_QT_RESOLVER_H
