//
// Created by byter on 30.09.17.
//

#ifndef THREEPP_SHADERMATERIAL_H
#define THREEPP_SHADERMATERIAL_H

#include "Material.h"
#include "Shader.h"

namespace three {

/**
 * @author alteredq / http://alteredqualia.com/
 *
 * parameters = {
 *  defines: { "label" : "value" },
 *  uniforms: { "parameter1": { value: 1.0 }, "parameter2": { value2: 2 } },
 *
 *  fragmentShader: <string>,
 *  vertexShader: <string>,
 *
 *  wireframe: <boolean>,
 *  wireframeLinewidth: <float>,
 *
 *  lights: <bool>,
 *
 *  skinning: <bool>,
 *  morphTargets: <bool>,
 *  morphNormals: <bool>
 * }
 */
class ShaderMaterial : public Material
{
public:
  std::unordered_map<std::string, std::string> defines;

  gl::UniformValues uniforms;

  bool clipping = false; // set to use user-defined clipping planes
  bool morphNormals = false; // set to use morph normals
  
  std::string vertexShader = "void main() {\n\tgl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );\n}";
  std::string fragmentShader = "void main() {\n\tgl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );\n}";

  // When rendered geometry doesn't include these attributes but the material does,
  // use these default values in WebGL. This avoids errors when buffer data is missing.
  const math::Vector3 default_color = {1, 1, 1};
  const math::Vector2 default_uv = {0.0f, 0.0f};
  const math::Vector2 default_uv2 = {0.0f, 0.0f};

protected:
  ShaderMaterial(const material::Info &info, const material::Typer &typer,
                 const gl::UniformValues &uniforms,
                 const char *vertexShader,
                 const char *fragmentShader,
                 Side side, bool depthTest,
                 bool depthWrite,
                 bool fog)
     : Material(info, typer),
       vertexShader(vertexShader), fragmentShader(fragmentShader), uniforms(uniforms)
  {
    this->depthTest = depthTest;
    this->depthWrite = depthWrite;
    this->fog = fog;
    this->side = side;
  }

  ShaderMaterial(const gl::UniformValues &uniforms,
                 const char *vertexShader,
                 const char *fragmentShader,
                 Side side,
                 bool depthTest,
                 bool depthWrite,
                 bool fog)
     : ShaderMaterial(material::InfoT<ShaderMaterial>(), material::Typer(this),
                      uniforms, vertexShader, fragmentShader, side, depthTest, depthWrite, fog)
  {
  }

  ShaderMaterial(Shader &shader, Side side, bool depthTest, bool depthWrite, bool fog)
     : ShaderMaterial(material::InfoT<ShaderMaterial>(), material::Typer(this),
                      shader.uniforms(), shader.vertexShader(), shader.fragmentShader(), side, depthTest, depthWrite, fog)
  {
  }

  ShaderMaterial(const ShaderMaterial &material)
     : Material(material, material::InfoT<ShaderMaterial>(), material::Typer(this)),
       defines(material.defines), uniforms(material.uniforms),
       clipping(material.clipping), morphNormals(material.morphNormals),
       vertexShader(material.vertexShader), fragmentShader(material.fragmentShader)
  {
  }

  ShaderMaterial(bool morphTargets, bool skinning)
     : Material(material::InfoT<ShaderMaterial>(), material::Typer(this))
  {
    unsigned linewidth = 1;

    this->morphTargets = morphTargets;
    this->skinning = skinning;

    wireframe = false;
    wireframeLineWidth = 1;

    fog = false; // set to use scene fog
    lights = false; // set to use scene lights
  }

public:
  bool use_derivatives = false; // set to use derivatives
  bool use_fragDepth = false; // set to use fragment depth values
  bool use_drawBuffers = false; // set to use draw buffers
  bool use_shaderTextureLOD = false; // set to use shader texture LOD

  bool uniformsNeedUpdate = false;

  std::string index0AttributeName;

  using Ptr = std::shared_ptr<ShaderMaterial>;
  static Ptr make(bool morphTargets, bool skinning) {
    return Ptr(new ShaderMaterial(morphTargets, skinning));
  }

  static Ptr make(Shader &shader, Side side, bool depthTest, bool depthWrite, bool fog) {
    return Ptr(new ShaderMaterial(shader, side, depthTest, depthWrite, fog));
  }

  static Ptr make(const gl::UniformValues &uniforms,
                  const char *vertexShader,
                  const char *fragmentShader,
                  Side side,
                  bool depthTest = true,
                  bool depthWrite = true,
                  bool fog = true)
  {
    return Ptr(new ShaderMaterial(uniforms, vertexShader, fragmentShader, side, depthTest, depthWrite, fog));
  }

  ShaderMaterial *cloned() const override {
    return new ShaderMaterial(*this);
  }
};

}
#endif //THREEPP_SHADERMATERIAL_H
