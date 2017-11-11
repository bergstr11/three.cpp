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
#include <renderers/Resolver.h>

namespace three {

class Camera;

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

  float opacity = 1;
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

  Texture::Ptr map;
  Texture::Ptr envMap;
  Texture::Ptr gradientMap;

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
  using Ptr = std::shared_ptr<Material>;

  virtual void setupPointLight(const math::Vector3 &position, float near, float far)
  {
  }
};

}
#endif //THREE_QT_MATERIAL_H
