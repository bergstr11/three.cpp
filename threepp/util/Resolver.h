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

struct Color;

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

struct Info {
  const gl::ShaderID shaderId;
  const char * const shaderName;

  Info(gl::ShaderID shaderId, const char *shaderName)
    : shaderId(shaderId), shaderName(shaderName) {}
};

template <typename Mat> struct InfoT;

#define MATERIAL_INFO(MaterialName, Shaderid) \
template <> struct InfoT<MaterialName> : public Info { \
  InfoT() : Info(Shaderid, #MaterialName) {} \
};

MATERIAL_INFO(MeshDepthMaterial, gl::ShaderID::depth)
MATERIAL_INFO(MeshDistanceMaterial, gl::ShaderID::distanceRGBA)
MATERIAL_INFO(MeshNormalMaterial, gl::ShaderID::normal)
MATERIAL_INFO(MeshBasicMaterial, gl::ShaderID::basic)
MATERIAL_INFO(MeshLambertMaterial, gl::ShaderID::lambert)
MATERIAL_INFO(MeshPhongMaterial, gl::ShaderID::phong)
MATERIAL_INFO(MeshToonMaterial, gl::ShaderID::phong)
MATERIAL_INFO(MeshStandardMaterial, gl::ShaderID::standard)
MATERIAL_INFO(MeshPhysicalMaterial, gl::ShaderID::physical)
MATERIAL_INFO(LineBasicMaterial, gl::ShaderID::basic)
MATERIAL_INFO(LineDashedMaterial, gl::ShaderID::dashed)
MATERIAL_INFO(PointsMaterial, gl::ShaderID::points)
MATERIAL_INFO(ShadowMaterial, gl::ShaderID::shadow)
MATERIAL_INFO(ShaderMaterial, gl::ShaderID::undefined)
MATERIAL_INFO(RawShaderMaterial, gl::ShaderID::undefined)
MATERIAL_INFO(SpriteMaterial, gl::ShaderID::undefined)

}

#define CAST(x, y, Cls) auto y = std::dynamic_pointer_cast<Cls>(x)
#define CAST2(x, Cls) std::dynamic_pointer_cast<Cls>(x)
}

#endif //THREEPP_RESOLVER_H
