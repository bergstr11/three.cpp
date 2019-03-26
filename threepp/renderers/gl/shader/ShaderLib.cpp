//
// Created by byter on 18.10.17.
//

#include "ShaderLib.h"

#include <QResource>
#include <QFile>

#include <threepp/core/Color.h>
#include <threepp/math/Vector3.h>

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

  QFile _vertex;
  QByteArray _vertexShader;

  QFile _fragment;
  QByteArray _fragmentShader;

public:
  LibShader(ShaderID id, const uniformslib::LibUniformValues &uniforms, const char *vertexShader, const char *fragmentShader)
     : _uniforms(uniforms), _id(id), _vertex(vertexShader), _fragment(fragmentShader)
  {}

  LibShader(const LibShader &shader)
     : _id(shader._id), _uniforms(shader._uniforms), _vertex(shader._vertex.fileName()), _fragment(shader._fragment.fileName()),
       _vertexShader(shader._vertexShader), _fragmentShader(shader._fragmentShader)
  {}

  const uniformslib::LibUniformValues &uniforms() const {
    return _uniforms;
  }

  const char *vertexShader()
  {
    if (!_vertexShader.isNull()) return _vertexShader.data();

    if(_vertex.open(QIODevice::ReadOnly)) {
      _vertexShader = _vertex.readAll();
      _vertex.close();
    }
    else
      throw std::logic_error("shader resource not available");

    return _vertexShader.data();
  }

  const char *fragmentShader()
  {
    if (!_fragmentShader.isNull()) return _fragmentShader.data();

    if(_fragment.open(QIODevice::ReadOnly)) {
      _fragmentShader = _fragment.readAll();
      _fragment.close();
    }
    else
      throw std::logic_error("shader resource not available");

    return _fragmentShader.data();
  }
};

class ShaderLib
{
  enum_map<ShaderID, three::gl::LibShader> _shaders;

  void add(ShaderID id, const three::gl::LibShader &sh)
  {
    _shaders.emplace(id, sh);
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
                  ":shader/meshbasic_vert.glsl",
                  ":shader/meshbasic_frag.glsl"
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
                     {UniformName::emissive, Color(ColorName::black)}
                  }),
                  ":shader/meshlambert_vert.glsl",
                  ":shader/meshlambert_frag.glsl"
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
                     {UniformName::emissive, Color(ColorName::black)},
                     {UniformName::specular, Color(ColorName::white)},
                     {UniformName::shininess, 30.0f}
                  }),
                  ":shader/meshphong_vert.glsl",
                  ":shader/meshphong_frag.glsl"
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
                     {UniformName::emissive,        Color(ColorName::black)},
                     {UniformName::roughness,       0.5f},
                     {UniformName::metalness,       0.5f},
                     {UniformName::envMapIntensity, 1}
                  }),
                  ":shader/meshphysical_vert.glsl",
                  ":shader/meshphysical_frag.glsl"
        ));
    add(ShaderID::physical,
        LibShader(ShaderID::physical,
        _shaders.at(ShaderID::standard).uniforms().merge(
           {
             {UniformName::clearCoat, 0.0f},
             {UniformName::clearCoatRoughness, 0.0f}
           }),
           ":shader/meshphysical_vert.glsl",
           ":shader/meshphysical_frag.glsl"
        ));
    add(ShaderID::points,
        LibShader(ShaderID::points,
                  uniformslib::merged(
                     {
                        UniformsID::points,
                        UniformsID::fog
                     }
                  ),
                  ":shader/points_vert.glsl",
                  ":shader/points_frag.glsl"
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
                  ":shader/linedashed_vert.glsl",
                  ":shader/linedashed_frag.glsl"
        ));
    add(ShaderID::depth,
        LibShader(ShaderID::depth,
                  uniformslib::merged(
                     {
                        UniformsID::common,
                        UniformsID::displacementmap
                     }
                  ),
                  ":shader/depth_vert.glsl",
                  ":shader/depth_frag.glsl"
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
                  ":shader/normal_vert.glsl",
                  ":shader/normal_frag.glsl"
        ));
    add(ShaderID::cube,
        LibShader(ShaderID::cube,
                  {
                     uniformslib::value<CubeTexture::Ptr>(UniformName::tCube, nullptr),
                     uniformslib::value<float>(UniformName::tFlip, -1.0f),
                     uniformslib::value<float>(UniformName::opacity, 1.0f)
                  },
                  ":shader/cube_vert.glsl",
                  ":shader/cube_frag.glsl"
        ));
    add(ShaderID::equirect,
        LibShader(ShaderID::equirect,
                  {
                     uniformslib::value<Texture::Ptr>(UniformName::tEquirect, nullptr)
                  },
                  ":shader/equirect_vert.glsl",
                  ":shader/equirect_frag.glsl"
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
                     {UniformName::nearDistance,      1.0f},
                     {UniformName::farDistance,       1000.0f},
                  }),
                  ":shader/distanceRGBA_vert.glsl",
                  ":shader/distanceRGBA_frag.glsl"
        ));
    add(ShaderID::shadow,
        LibShader(ShaderID::shadow,
                  uniformslib::merged(
                     {
                        UniformsID::lights,
                        UniformsID::fog
                     }
                  ).merge(UniformsID::fog, {
                     {UniformName::color,   Color(ColorName::black)},
                     {UniformName::opacity, 1.0f}
                  }),
                  ":shader/shadow_vert.glsl",
                  ":shader/shadow_frag.glsl"
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

#define MATCH_NAME(nm) {#nm, ShaderID::nm}

ShaderID toShaderID(std::string name)
{
  static std::unordered_map<std::string, ShaderID> string_to_id {
     MATCH_NAME(basic),
     MATCH_NAME(lambert),
     MATCH_NAME(phong),
     MATCH_NAME(standard),
     MATCH_NAME(normal),
     MATCH_NAME(points),
     MATCH_NAME(dashed),
     MATCH_NAME(depth),
     MATCH_NAME(cube),
     MATCH_NAME(equirect),
     MATCH_NAME(distanceRGBA),
     MATCH_NAME(shadow),
     MATCH_NAME(physical)
  };
  try {
    return string_to_id.at(name);
  }
  catch(const std::out_of_range &r) {
    throw std::invalid_argument(std::string("unknown variable ")+name);
  }
}

}
}
