//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_CAPABILITIES_H
#define THREE_QT_CAPABILITIES_H

#include <QOpenGLFunctions>;
#include "Extensions.h"
#include "Helpers.h"

namespace three {
namespace gl {

struct Capabilities
{
  const Extensions &_extensions;
  const Parameters &_parameters;
  QOpenGLFunctions * const _fn;

  bool logarithmicDepthBuffer;

  GLint maxTextures;
  GLint maxVertexTextures;
  GLsizei maxTextureSize;
  GLsizei maxCubemapSize;

  GLint maxAttributes;
  GLint maxVertexUniforms;
  GLint maxVaryings;
  GLint maxFragmentUniforms;

  bool vertexTextures;
  bool floatFragmentTextures;
  bool floatVertexTextures;

  GLint maxAnisotropy = -1;
  Precision maxPrecision;

public:
  void Capabilities(QOpenGLFunctions *fn, const Extensions &extensions, const Parameters &parameters)
  : _fn(fn), _extensions(extensions), _parameters(parameters)
  { }

  void init()
  {
    logarithmicDepthBuffer = _parameters.logarithmicDepthBuffer && _extensions[Extension::EXT_frag_depth];
    _fn->glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
    _fn->glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextures);
    _fn->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    _fn->glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubemapSize);
    _fn->glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);
    _fn->glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniforms);
    _fn->glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVaryings);
    _fn->glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFragmentUniforms);

    vertexTextures = maxVertexTextures > 0;
    floatFragmentTextures = _extensions.get(Extension::OES_texture_float);
    floatVertexTextures = vertexTextures && floatFragmentTextures;

    Precision precision = _parameters.precision;
    maxPrecision = getMaxPrecision( precision );

    if ( maxPrecision != precision ) {
      //warn( 'THREE.WebGLRenderer:', precision, 'not supported, using', maxPrecision, 'instead.' );
      precision = maxPrecision;
    }
  }

  GLint getMaxAnisotropy()
  {
    if (maxAnisotropy < 0) return maxAnisotropy;

    if (_extensions.get(Extension::EXT_texture_filter_anisotropic))
      _fn->glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    else
      maxAnisotropy = 0;

    return maxAnisotropy;

  }

  Precision getMaxPrecision(Precision precision)
  {
    GLint r, p;

    if (precision == Precision::highp) {

      _fn->glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_HIGH_FLOAT, &r, &p);

      if (p > 0) {
        _fn->glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, &r, &p);

        if(p > 0)
          return Precision::highp;
      }
      precision = Precision::mediump;
    }

    if ( precision == Precision::mediump) {

      _fn->glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_MEDIUM_FLOAT, &r, &p);

      if (p > 0) {
        _fn->glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT, &r, &p);
        if(p > 0)
          return Precision::mediump;
      }
    }
    return Precision::lowp;
  }
};

}
}
#endif //THREE_QT_CAPABILITIES_H
