//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_PROGRAMS_H
#define THREE_QT_PROGRAMS_H

#include <objects/SkinnedMesh.h>
#include "Program.h"
#include "Lights.h"
#include "Extensions.h"
#include "Capabilities.h"

namespace three {
namespace gl {

class Renderer_impl;

class ProgramParameters {
  ProgramParameters() {};

public:
  using Ptr = std::shared_ptr<ProgramParameters>;
  static Ptr make() {return Ptr(new ProgramParameters());}

  std::string shaderID;
  Precision precision;
  bool supportsVertexTextures;
  Encoding outputEncoding;
  bool hasMap;
  Encoding mapEncoding;
  bool hasEnvMap;
  Encoding envMapEncoding;
  TextureMapping envMapMode;
  bool envMapCubeUV;
  bool lightMap;
  bool aoMap;
  bool emissiveMap;
  Encoding emissiveMapEncoding;
  bool bumpMap;
  bool normalMap;
  bool displacementMap;
  bool roughnessMap;
  bool metalnessMap;
  bool specularMap;
  bool alphaMap;
  bool gradientMap;
  CombineOperation combine;
  Colors vertexColors;
};

class Programs
{
  std::vector<Program::Ptr> _programs;

  Renderer_impl &_renderer;
  Extensions &_extensions;
  Capabilities &_capabilities;

public:
  Programs(Renderer_impl &renderer, Extensions &extensions, Capabilities &capabilities)
     : _renderer(renderer), _extensions(extensions), _capabilities(capabilities) {}

  unsigned allocateBones(SkinnedMesh::Ptr object) 
  {
    const Skeleton::Ptr skeleton = object->skeleton();
    const std::vector<Bone::Ptr> bones = skeleton->bones();

    if (_capabilities.floatVertexTextures ) {
      return 1024;
    }
    else {

      // default for when object is not specified
      // ( for example when prebuilding shader to be used with multiple objects )
      //
      //  - leave some extra space for other uniforms
      //  - limit here is ANGLE's 254 max uniform vectors
      //    (up to 54 should be safe)

      //var nVertexUniforms = _capabilities.maxVertexUniforms;
      size_t nVertexMatrices = (size_t)std::floor((_capabilities.maxVertexUniforms - 20) / 4 );

      size_t maxBones = std::min( nVertexMatrices, bones.size());

      if (maxBones < bones.size()) {

        //Skeleton has ' + bones.length + ' bones. This GPU supports ' + maxBones + '.' );
        return 0;
      }
      return maxBones;
    }
  }

  ProgramParameters::Ptr getParameters(Material::Ptr material,
                                Lights::State &lights,
                                const std::vector<Light::Ptr> &shadows,
                                const Fog::Ptr fog,
                                size_t nClipPlanes,
                                size_t nClipIntersection,
                                Object3D::Ptr object );
#if 0
  void getProgramCode(Material::Ptr material, parameters ) {

    var array = [];

    if ( parameters.shaderID ) {

      array.push( parameters.shaderID );

    } else {

      array.push( material.fragmentShader );
      array.push( material.vertexShader );

    }

    if ( material.defines !== undefined ) {

      for ( var name in material.defines ) {

        array.push( name );
        array.push( material.defines[ name ] );

      }

    }

    for ( var i = 0; i < parameterNames.length; i ++ ) {

      array.push( parameters[ parameterNames[ i ] ] );

    }

    array.push( material.onBeforeCompile.toString() );

    array.push( renderer.gammaOutput );

    return array.join();

  }
#endif
#if 0
  void acquireProgram ( material, shader, parameters, code ) {

    var program;

    // Check if code has been already compiled
    for ( var p = 0, pl = programs.length; p < pl; p ++ ) {

      var programInfo = programs[ p ];

      if ( programInfo.code === code ) {

        program = programInfo;
        ++ program.usedTimes;

        break;

      }

    }

    if ( program === undefined ) {

      program = new WebGLProgram( renderer, extensions, code, material, shader, parameters );
      programs.push( program );

    }

    return program;

  }

  void releaseProgram( program ) {

    if ( -- program.usedTimes === 0 ) {

      // Remove from unordered set
      var i = programs.indexOf( program );
      programs[ i ] = programs[ programs.length - 1 ];
      programs.pop();

      // Free WebGL resources
      program.destroy();

    }

  }
#endif
};
}
}
#endif //THREE_QT_PROGRAMS_H
