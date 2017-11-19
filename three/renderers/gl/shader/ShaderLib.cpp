//
// Created by byter on 18.10.17.
//

#include "ShaderLib.h"
#include <core/Color.h>
#include <math/Vector3.h>

static void qInitResource()
{
  static bool done = false;

  if (done) return;
  done = true;

  //must do this outside of namespace - hence this function
  Q_INIT_RESOURCE(ShaderLib);
  Q_INIT_RESOURCE(ShaderChunk);
}

namespace three {
namespace gl {

class LibShader
{
  ShaderID _id;
  uniformslib::LibUniformValues _uniforms;

  QResource _vertex;
  QByteArray _uncompressedVertex;
  const char *_vertexShader = nullptr;

  QResource _fragment;
  QByteArray _uncompressedFragment;
  const char *_fragmentShader = nullptr;

public:
  LibShader(ShaderID id, const uniformslib::LibUniformValues &uniforms, const char *vertexShader, const char *fragmentShader)
     : _uniforms(uniforms), _id(id), _vertex(vertexShader), _fragment(fragmentShader)
  {}

  LibShader(const LibShader &shader)
     : _uniforms(shader._uniforms), _id(shader._id), _vertex(shader._vertexShader), _fragment(shader._fragmentShader)
  {}

  const uniformslib::LibUniformValues &uniforms() const {
    return _uniforms;
  }

  const char *vertexShader()
  {
    if (_vertexShader) return _vertexShader;

    if (_vertex.isCompressed()) {
      _uncompressedVertex = qUncompress(_vertex.data(), _vertex.size());
      _vertexShader = _uncompressedVertex.data();
    }
    else
      _vertexShader = (const char *) _vertex.data();

    return _vertexShader;
  }

  const char *fragmentShader()
  {
    if (_fragmentShader) return _fragmentShader;

    if (_fragment.isCompressed()) {
      _uncompressedFragment = qUncompress(_fragment.data(), _fragment.size());
      _fragmentShader = _uncompressedFragment.data();
    }
    else
      _fragmentShader = (const char *) _fragment.data();

    return _fragmentShader;
  }
};

class ShaderLib
{
  std::unordered_map<ShaderID, three::gl::LibShader> _shaders;

  void add(ShaderID id, const three::gl::LibShader &sh)
  {
    _shaders.insert({id, sh});
  }

public:
  ShaderLib()
  {
    qInitResource();

    add(ShaderID::basic,
        LibShader(ShaderID::basic,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::specularmap,
                        UniformsID::envmap,
                        UniformsID::aomap,
                        UniformsID::lightmap,
                        UniformsID::fog
                     }
                  ),
                  ":chunk/cube_vert.glsl",
                  ":chunk/cube_frag.glsl"
        ));
    add(ShaderID::lambert,
        LibShader(ShaderID::lambert,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::specularmap,
                        UniformsID::envmap,
                        UniformsID::aomap,
                        UniformsID::lightmap,
                        UniformsID::emissivemap,
                        UniformsID::fog,
                        UniformsID::lights
                     }
                  ).merge(UniformsID::lights, {
                     {UniformName::emissive, Color()}
                  }),
                  ":chunk/meshlambert_vert.glsl",
                  ":chunk/meshlambert_frag.glsl"
        ));
    add(ShaderID::phong,
        LibShader(ShaderID::phong,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::specularmap,
                        UniformsID::envmap,
                        UniformsID::aomap,
                        UniformsID::lightmap,
                        UniformsID::emissivemap,
                        UniformsID::bumpmap,
                        UniformsID::normalmap,
                        UniformsID::displacementmap,
                        UniformsID::gradientmap,
                        UniformsID::fog,
                        UniformsID::lights
                     }
                  ).merge(UniformsID::lights, {
                     {UniformName::emissive, Color(0x000000)},
                     {UniformName::specular, Color(0x111111)},
                     {UniformName::emissive, 30}
                  }),
                  ":chunk/meshphong_vert.glsl",
                  ":chunk/meshphong_frag.glsl"
        ));
    add(ShaderID::standard,
        LibShader(ShaderID::standard,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::envmap,
                        UniformsID::aomap,
                        UniformsID::lightmap,
                        UniformsID::emissivemap,
                        UniformsID::bumpmap,
                        UniformsID::normalmap,
                        UniformsID::displacementmap,
                        UniformsID::roughnessmap,
                        UniformsID::metalnessmap,
                        UniformsID::fog
                     }
                  ).merge(UniformsID::lights, {
                     {UniformName::emissive,        Color(0x000000)},
                     {UniformName::roughness,       0.5f},
                     {UniformName::metalness,       0.5f},
                     {UniformName::envMapIntensity, 1}
                  }),
                  ":chunk/meshphysical_vert.glsl",
                  ":chunk/meshphysical_frag.glsl"
        ));
    add(ShaderID::points,
        LibShader(ShaderID::points,
                  uniformslib::merged(
                     {
                        UniformsID::points,
                        UniformsID::fog
                     }
                  ),
                  ":chunk/points_vert.glsl",
                  ":chunk/points_frag.glsl"
        ));
    add(ShaderID::dashed,
        LibShader(ShaderID::dashed,
                  uniformslib::merged(
                     {
                        UniformsID::common
                     }
                  ).merge(UniformsID::fog, {
                     {UniformName::scale,     1},
                     {UniformName::dashSize,  1},
                     {UniformName::totalSize, 2}
                  }),
                  ":chunk/linedashed_vert.glsl",
                  ":chunk/linedashed_frag.glsl"
        ));
    add(ShaderID::depth,
        LibShader(ShaderID::depth,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::displacementmap
                     }
                  ),
                  ":chunk/depth_vert.glsl",
                  ":chunk/depth_frag.glsl"
        ));
    add(ShaderID::normal,
        LibShader(ShaderID::normal,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::bumpmap,
                        UniformsID::normalmap,
                        UniformsID::displacementmap
                     }
                  ).merge(UniformsID::displacementmap, {
                     {UniformName::opacity, 1.0f},
                  }),
                  ":chunk/normal_vert.glsl",
                  ":chunk/normal_frag.glsl"
        ));
    add(ShaderID::cube,
        LibShader(ShaderID::cube,
                  {
                     uniformslib::value<CubeTexture::Ptr>(UniformName::cube, nullptr),
                     uniformslib::value<GLint>(UniformName::flip, -1),
                     uniformslib::value<float>(UniformName::opacity, 1.0f)
                  },
                  ":chunk/cube_vert.glsl",
                  ":chunk/cube_frag.glsl"
        ));
    add(ShaderID::equirect,
        LibShader(ShaderID::equirect,
                  {
                     uniformslib::value<DefaultTexture::Ptr>(UniformName::equirect, nullptr)
                  },
                  ":chunk/equirect_vert.glsl",
                  ":chunk/equirect_frag.glsl"
        ));
    add(ShaderID::distanceRGBA,
        LibShader(ShaderID::distanceRGBA,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::displacementmap
                     }
                  ).merge(UniformsID::displacementmap, {
                     {UniformName::referencePosition, math::Vector3()},
                     {UniformName::nearDistance,      1},
                     {UniformName::farDistance,       1},
                  }),
                  ":chunk/distanceRGBA_vert.glsl",
                  ":chunk/distanceRGBA_frag.glsl"
        ));
    add(ShaderID::shadow,
        LibShader(ShaderID::shadow,
                  uniformslib::merged(
                     {
                        UniformsID::lights,
                        UniformsID::fog
                     }
                  ).merge(UniformsID::fog, {
                     {UniformName::color,   Color(0x00000)},
                     {UniformName::opacity, 1.0f}
                  }),
                  ":chunk/distanceRGBA_vert.glsl",
                  ":chunk/distanceRGBA_frag.glsl"
        ));
  }

  LibShader &operator[](ShaderID id)
  {
    return _shaders.at(id);
  }
};

namespace shaderlib {

LibShader &_get(ShaderID id)
{
  static ShaderLib shaders;

  return shaders[id];
}

three::gl::ShaderInfo get(ShaderID id)
{
  LibShader &lib = _get(id);
  return three::gl::ShaderInfo(lib.uniforms(), lib.vertexShader(), lib.fragmentShader());
}

}

ShaderInfo::ShaderInfo(const uniformslib::LibUniformValues &uniforms, const char * vertexShader, const char * fragmentShader)
   : uniforms(uniforms), vertexShader(vertexShader), fragmentShader(fragmentShader)
{}

}
}
