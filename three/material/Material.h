//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_MATERIAL_H
#define THREE_QT_MATERIAL_H

#include <memory>
#include <Constants.h>
#include <textures/Texture.h>

namespace three {

struct Material
{
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

  //clippingPlanes = null;
  bool clipIntersection = false;
  bool clipShadows = false;

  bool colorWrite = true;

  //precision = null; // override the renderer's default precision for this material

  bool polygonOffset = false;
  unsigned polygonOffsetFactor = 0;
  unsigned polygonOffsetUnits = 0;

  bool dithering = false;

  unsigned alphaTest = 0;
  bool premultipliedAlpha = false;

  bool _skinning = false;
  bool _morphTargets = false;

  unsigned overdraw = 0; // Overdrawn pixels (typically between 0 and 1) for fixing antialiasing gaps in CanvasRenderer

  bool visible = true;

  bool needsUpdate = true;

  Texture::Ptr _map;

  Texture::Ptr _alphaMap;

  Texture::Ptr _displacementMap;

  float _displacementScale = 1;
  float _displacementBias = 0;

  bool _fog = false;
  bool _lights = false;

protected:
  Material(bool morphTargets, bool skinning) : _skinning(skinning), _morphTargets(morphTargets)
  {}

public:
  using Ptr = std::shared_ptr<Material>;

  uint16_t id() const {return _id;}

  bool morphTargets() const {return _morphTargets;}
};

}
#endif //THREE_QT_MATERIAL_H
