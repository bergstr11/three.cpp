//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_PROGRAMS_H
#define THREE_QT_PROGRAMS_H

#include <objects/SkinnedMesh.h>
#include "Program.h"
#include "Extensions.h"
#include "Capabilities.h"

namespace three {
namespace gl {

class Programs
{
  std::vector<Program::Ptr> _programs;
  Extensions &_extensions;
  Capabilities &_capabilities;

public:
  Programs(Extensions &extensions, Capabilities &capabilities)
     : _extensions(extensions), _capabilities(capabilities) {}

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

  std::string getShaderId(Material::Ptr material)
  {
    static const material::ShaderIDs ids;

    return ids.string(*material);
  }

  void getParameters(Material::Ptr material,
                     Lights::State &lights,
                     const std::vector<Light::Ptr> &shadows,
                     const Fog::Ptr fog,
                     size_t nClipPlanes,
                     size_t nClipIntersection,
                     Object3D::Ptr object )
  {
#if 0
    var shaderID = shaderIDs[ material->type ];

    // heuristics to create shader parameters according to lights in the scene
    // (not to blow over maxLights budget)

    var maxBones = object.isSkinnedMesh ? allocateBones( object ) : 0;
    var precision = capabilities.precision;

    if ( material.precision !== null ) {

      precision = capabilities.getMaxPrecision( material.precision );

      if ( precision !== material.precision ) {

        console.warn( 'THREE.WebGLProgram.getParameters:', material.precision, 'not supported, using', precision, 'instead.' );

      }

    }

    var currentRenderTarget = renderer.getRenderTarget();

    var parameters = {

       shaderID: shaderID,

       precision: precision,
       supportsVertexTextures: capabilities.vertexTextures,
       outputEncoding: getTextureEncodingFromMap( ( ! currentRenderTarget ) ? null : currentRenderTarget.texture, renderer.gammaOutput ),
       map: !! material.map,
       mapEncoding: getTextureEncodingFromMap( material.map, renderer.gammaInput ),
       envMap: !! material.envMap,
       envMapMode: material.envMap && material.envMap.mapping,
       envMapEncoding: getTextureEncodingFromMap( material.envMap, renderer.gammaInput ),
       envMapCubeUV: ( !! material.envMap ) && ( ( material.envMap.mapping === CubeUVReflectionMapping ) || ( material.envMap.mapping === CubeUVRefractionMapping ) ),
       lightMap: !! material.lightMap,
       aoMap: !! material.aoMap,
       emissiveMap: !! material.emissiveMap,
       emissiveMapEncoding: getTextureEncodingFromMap( material.emissiveMap, renderer.gammaInput ),
       bumpMap: !! material.bumpMap,
       normalMap: !! material.normalMap,
       displacementMap: !! material.displacementMap,
       roughnessMap: !! material.roughnessMap,
       metalnessMap: !! material.metalnessMap,
       specularMap: !! material.specularMap,
       alphaMap: !! material.alphaMap,

       gradientMap: !! material.gradientMap,

       combine: material.combine,

       vertexColors: material.vertexColors,

       fog: !! fog,
       useFog: material.fog,
       fogExp: ( fog && fog.isFogExp2 ),

       flatShading: material.flatShading,

       sizeAttenuation: material.sizeAttenuation,
       logarithmicDepthBuffer: capabilities.logarithmicDepthBuffer,

       skinning: material.skinning && maxBones > 0,
       maxBones: maxBones,
       useVertexTexture: capabilities.floatVertexTextures,

       morphTargets: material.morphTargets,
       morphNormals: material.morphNormals,
       maxMorphTargets: renderer.maxMorphTargets,
       maxMorphNormals: renderer.maxMorphNormals,

       numDirLights: lights.directional.length,
       numPointLights: lights.point.length,
       numSpotLights: lights.spot.length,
       numRectAreaLights: lights.rectArea.length,
       numHemiLights: lights.hemi.length,

       numClippingPlanes: nClipPlanes,
       numClipIntersection: nClipIntersection,

       dithering: material.dithering,

       shadowMapEnabled: renderer.shadowMap.enabled && object.receiveShadow && shadows.length > 0,
       shadowMapType: renderer.shadowMap.type,

       toneMapping: renderer.toneMapping,
       physicallyCorrectLights: renderer.physicallyCorrectLights,

       premultipliedAlpha: material.premultipliedAlpha,

       alphaTest: material.alphaTest,
       doubleSided: material.side === DoubleSide,
       flipSided: material.side === BackSide,

       depthPacking: ( material.depthPacking !== undefined ) ? material.depthPacking : false

    };
    return parameters;
#endif

  }
#if 0
  void getProgramCode = function ( material, parameters ) {

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
