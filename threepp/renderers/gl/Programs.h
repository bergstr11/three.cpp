//
// Created by byter on 02.10.17.
//

#ifndef THREEPP_PROGRAMS_H
#define THREEPP_PROGRAMS_H

#include <threepp/objects/SkinnedMesh.h>
#include "Program.h"
#include "Lights.h"
#include "Extensions.h"
#include "Capabilities.h"

namespace three {
namespace gl {

class Renderer_impl;

class Programs
{
  std::unordered_map<ProgramParameters::Ptr, Program::Ptr, parameters_hash, parameters_equal> _programs;

  Renderer_impl &_renderer;
  Extensions &_extensions;
  Capabilities &_capabilities;

public:
  Programs(Renderer_impl &renderer, Extensions &extensions, Capabilities &capabilities)
     : _renderer(renderer), _extensions(extensions), _capabilities(capabilities) {}

  unsigned allocateBones(SkinnedMesh *skinnedMesh)
  {
    const Skeleton::Ptr skeleton = skinnedMesh->skeleton();
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

  ProgramParameters::Ptr getParameters(const Renderer_impl &renderer,
                                       Material::Ptr material,
                                       Lights::State &lights,
                                       const std::vector<Light::Ptr> &shadows,
                                       const Fog::Ptr fog,
                                       size_t nClipPlanes,
                                       size_t nClipIntersection,
                                       Object3D::Ptr object);

  Program::Ptr acquireProgram (Material::Ptr material, Shader &shader, ProgramParameters::Ptr parameters)
  {
    // Check if code has been already compiled
    auto it = _programs.find(parameters);
    if(it != _programs.end()) return it->second;

    Program::Ptr program = Program::make( _renderer, _extensions, material, shader, parameters );
    _programs[parameters] = program;

    return program;
  }

  void releaseProgram(Program::Ptr program)
  {
    if (program.use_count() == 2) {

      //should call the program destructor
      _programs.erase(program->parameters);
    }

  }
};

}
}
#endif //THREEPP_PROGRAMS_H
