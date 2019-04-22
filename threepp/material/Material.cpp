//
// Created by byter on 26.09.17.
//

#include <threepp/camera/Camera.h>
#include "MeshBasicMaterial.h"
#include "MeshDistanceMaterial.h"
#include "MeshDepthMaterial.h"
#include "MeshLambertMaterial.h"
#include "MeshNormalMaterial.h"
#include "MeshPhongMaterial.h"
#include "MeshStandardMaterial.h"
#include "MeshToonMaterial.h"
#include "PointsMaterial.h"

namespace three {

uint16_t Material::___material_id_count = 0;

Material::Material(const Material &material, const material::Info &info, const material::Typer &typer)
   : uuid(sole::uuid4()), id(___material_id_count++), info(info), typer(typer)
{
  fog = material.fog;
  lights = material.lights;

  ambientColor = material.ambientColor;

  blending = material.blending;
  side = material.side;
  flatShading = material.flatShading;
  vertexColors = material.vertexColors;

  blendSrc = material.blendSrc;
  blendDst = material.blendDst;
  blendEquation = material.blendEquation;

  blendSrcAlpha = material.blendSrcAlpha;
  blendDstAlpha = material.blendDstAlpha;
  blendEquationAlpha = material.blendEquationAlpha;

  depthFunc = material.depthFunc;
  depthTest = material.depthTest;
  depthWrite = material.depthWrite;

  clippingPlanes = material.clippingPlanes;
  clipIntersection = material.clipIntersection;
  clipShadows = material.clipShadows;

  colorWrite = material.colorWrite;

  wireframe = material.wireframe;
  wireframeLineWidth = material.wireframeLineWidth;
  wireframeLineCap = material.wireframeLineCap;
  wireframeLineJoin = material.wireframeLineJoin;

  precision = material.precision;

  polygonOffset = material.polygonOffset;
  polygonOffsetFactor = material.polygonOffsetFactor;
  polygonOffsetUnits = material.polygonOffsetUnits;

  dithering = material.dithering;

  alphaTest = material.alphaTest;
  premultipliedAlpha = material.premultipliedAlpha;

  skinning = material.skinning;
  morphTargets = material.morphTargets;
  numSupportedMorphTargets = material.numSupportedMorphTargets;
  morphNormals = material.morphNormals;
  numSupportedMorphNormals = material.numSupportedMorphNormals;

  overdraw = material.overdraw;

  visible = material.visible;

  needsUpdate = true;
}

}
