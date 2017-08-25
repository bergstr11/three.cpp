//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_MATERIAL_H
#define THREE_QT_MATERIAL_H

#include <memory>
#include <Constants.h>

namespace three {

class Material
{
  bool _fog = true;
  bool _lights = true;

  Blending _blending = Blending::Normal;
  Side _side = Side::Front;
  bool _flatShading = false;
  Colors _vertexColors = Colors::None;

  float _opacity = 1;
  bool _transparent = false;

  BlendFactor _blendSrc = BlendFactor::SrcAlpha;
  BlendFactor _blendDst = BlendFactor::OneMinusSrcAlpha;
  BlendEquation _blendEquation = BlendEquation::Add;

  BlendFactor _blendSrcAlpha = BlendFactor::None;
  BlendFactor _blendDstAlpha = BlendFactor::None;
  BlendEquation _blendEquationAlpha = BlendEquation::None;

  DepthFunc _depthFunc = DepthFunc::LessEqual;
  bool _depthTest = true;
  bool _depthWrite = true;

  //_clippingPlanes = null;
  bool _clipIntersection = false;
  bool _clipShadows = false;

  bool _colorWrite = true;

  //_precision = null; // override the renderer's default precision for this material

  bool _polygonOffset = false;
  unsigned _polygonOffsetFactor = 0;
  unsigned _polygonOffsetUnits = 0;

  bool _dithering = false;

  unsigned _alphaTest = 0;
  bool _premultipliedAlpha = false;

  unsigned _overdraw = 0; // Overdrawn pixels (typically between 0 and 1) for fixing antialiasing gaps in CanvasRenderer

  bool _visible = true;

  bool _needsUpdate = true;

public:
  using Ptr = std::shared_ptr<Material>;
};

}
#endif //THREE_QT_MATERIAL_H
