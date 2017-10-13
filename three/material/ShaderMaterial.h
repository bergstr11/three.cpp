//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_SHADERMATERIAL_H
#define THREE_QT_SHADERMATERIAL_H

#include "Material.h"

namespace three {

class ShaderMaterial : public Material
{
  //defines = {};
  //uniforms = {};

  const char *vertexShader = "void main() {\n\tgl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );\n}";
  const char * fragmentShader = "void main() {\n\tgl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );\n}";

  bool clipping = false; // set to use user-defined clipping planes
  bool morphNormals = false; // set to use morph normals

  bool ext_derivatives = false; // set to use derivatives
  bool ext_fragDepth = false; // set to use fragment depth values
  bool ext_drawBuffers = false; // set to use draw buffers
  bool ext_shaderTextureLOD = false; // set to use shader texture LOD

  // When rendered geometry doesn't include these attributes but the material does,
  // use these default values in WebGL. This avoids errors when buffer data is missing.
  math::Vector3 default_color = {1, 1, 1};
  math::Vector2 default_uv = {0.0f, 0.0f};
  math::Vector2 default_uv2 = {0.0f, 0.0f};

  std::string index0AttributeName;

  ShaderMaterial(bool morphTargets, bool skinning) : Material(morphTargets, false, skinning)
  {
    unsigned linewidth = 1;

    wireframe = false;
    wireframeLineWidth = 1;

    fog = false; // set to use scene fog
    lights = false; // set to use scene lights

    skinning = false; // set to use skinning attribute streams
    morphTargets = false; // set to use morph targets
  }

public:
  using Ptr = std::shared_ptr<ShaderMaterial>;
  static Ptr make(bool morphTargets, bool skinning) {
    return Ptr(new ShaderMaterial(morphTargets, skinning));
  }
};

}
#endif //THREE_QT_SHADERMATERIAL_H
