//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_MATERIAL_H
#define THREE_QT_MATERIAL_H

#include <memory>
#include <Constants.h>

namespace three {

struct Material
{
  bool fog = true;
  bool lights = true;

  Blending blending = Blending::Normal;
  Side side = Side::Front;
  bool flatShading = false;
  Colors vertexColors = Colors::None;

  float opacity = 1;
  bool transparent = false;

  BlendFactor blendSrc = BlendFactor::SrcAlpha;
  BlendFactor blendDst = BlendFactor::OneMinusSrcAlpha;
  BlendEquation blendEquation = BlendEquation::Add;

  BlendFactor blendSrcAlpha = BlendFactor::None;
  BlendFactor blendDstAlpha = BlendFactor::None;
  BlendEquation blendEquationAlpha = BlendEquation::None;

  DepthFunc depthFunc = DepthFunc::LessEqual;
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

  unsigned overdraw = 0; // Overdrawn pixels (typically between 0 and 1) for fixing antialiasing gaps in CanvasRenderer

  bool visible = true;

  bool needsUpdate = true;

protected:
  Material() {}

public:
  using Ptr = std::shared_ptr<Material>;
};

}
#endif //THREE_QT_MATERIAL_H
