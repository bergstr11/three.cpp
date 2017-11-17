//
// Created by byter on 11/17/17.
//

#ifndef THREE_QT_REFRESH_UNIFORMS_H
#define THREE_QT_REFRESH_UNIFORMS_H

#include <material/Material.h>

namespace three {
namespace gl {

template <typename ... Maps>
struct Refresh;

template <typename Map, typename ... Maps>
struct Refresh<Map, Maps...>
{
  template <typename ... Ms>
  static void mixin(UniformValues &u, MaterialT<Ms...> &m) {
    Refresh<Map>::mixin(u, m);
    Refresh<Maps...>::mixin(u, m);
  }
  static void handle(UniformValues &u, MaterialT<Map, Maps...> &m) {
    Refresh<Map>::mixin(u, m);
    Refresh<Maps...>::mixin(u, m);
  }
};

template <>
struct Refresh<Material>
{
  static void handle(UniformValues &u, MaterialT<Material> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, Material &m) {
    u[UniformName::opacity] = m.opacity;
  }
};
template <>
struct Refresh<material::LightMap>
{
  static void handle(UniformValues &u, MaterialT<material::LightMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::LightMap &m) {
  }
};
template <>
struct Refresh<material::EmissiveMap>
{
  static void handle(UniformValues &u, MaterialT<material::EmissiveMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::EmissiveMap &m) {
    u[UniformName::emissive] = m.emissive * m.emissiveIntensity;
  }
};
template <>
struct Refresh<material::AoMap>
{
  static void handle(UniformValues &u, MaterialT<material::AoMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::AoMap &m) {
  }
};
template <>
struct Refresh<material::EnvMap>
{
  static void handle(UniformValues &u, MaterialT<material::EnvMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::EnvMap &m) {
  }
};
template <>
struct Refresh<material::AlphaMap>
{
  static void handle(UniformValues &u, MaterialT<material::AlphaMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::AlphaMap &m) {
  }
};
template <>
struct Refresh<material::SpecularMap>
{
  static void handle(UniformValues &u, MaterialT<material::SpecularMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::SpecularMap &m) {
  }
};
template <>
struct Refresh<material::DisplacementMap>
{
  static void handle(UniformValues &u, MaterialT<material::DisplacementMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::DisplacementMap &m) {
  }
};
template <>
struct Refresh<material::BumpMap>
{
  static void handle(UniformValues &u, MaterialT<material::BumpMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::BumpMap &m) {
  }
};
template <>
struct Refresh<material::NormalMap>
{
  static void handle(UniformValues &u, MaterialT<material::NormalMap> &m) {
    mixin(u, m);
  }
  static void mixin(UniformValues &u, material::NormalMap &m) {
  }
};

template <typename ... Maps>
void refresh(UniformValues &u, MaterialT<Maps...> &m) {
  Refresh<Maps...>::handle(u, m);
}

}
}

#endif //THREE_QT_REFRESH_UNIFORMS_H
