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
    const Skeleton &skeleton = object->skeleton();
    const std::vector<Bone::Ptr> bones = skeleton.bones();

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

};

}
}
#endif //THREE_QT_PROGRAMS_H
