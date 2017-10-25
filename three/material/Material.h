//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_MATERIAL_H
#define THREE_QT_MATERIAL_H

#include <memory>
#include <Constants.h>
#include <textures/Texture.h>
#include <helper/sole.h>
#include <math/Plane.h>
#include <renderers/Resolver.h>

namespace three {

class Camera;

struct Material
{
  const sole::uuid uuid;
  uint16_t _id;

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

  //precision = null; // override the renderer's default precision for this material

  bool polygonOffset = false;
  unsigned polygonOffsetFactor = 0;
  unsigned polygonOffsetUnits = 0;

  bool dithering = false;

  unsigned alphaTest = 0;
  bool premultipliedAlpha = false;

  Texture::Ptr specularMap;

  Texture::Ptr alphaMap;

  Texture::Ptr envMap;

  bool _skinning = false;
  bool _morphTargets = false;
  bool _morphNormals = false;

  unsigned overdraw = 0; // Overdrawn pixels (typically between 0 and 1) for fixing antialiasing gaps in CanvasRenderer

  bool visible = true;

  bool needsUpdate = true;

  Texture::Ptr map;

  Texture::Ptr alphaMap;

  Texture::Ptr displacementMap;

  float displacementScale = 1;
  float displacementBias = 0;

  material::ResolverBase::Ptr resolver;

  Material(material::ResolverBase::Ptr resolver=material::Resolver<bool>::make(false))
     : uuid(sole::uuid0()), resolver(resolver) {}

  using Ptr = std::shared_ptr<Material>;

  uint16_t id() const {return _id;}

  bool skinning() const {return _skinning;}

  bool morphTargets() const {return _morphTargets;}

  bool morphNormals() const {return _morphNormals;}

  virtual void setupPointLight(const math::Vector3 &position, float near, float far)
  {
  }
};

}
#endif //THREE_QT_MATERIAL_H
