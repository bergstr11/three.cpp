//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_PROGRAMS_H
#define THREE_QT_PROGRAMS_H

#include <objects/SkinnedMesh.h>
#include "Program.h"

namespace three {
namespace gl {

class Programs
{
  std::vector<Program::Ptr> _programs;
  
public:
  unsigned allocateBones(SkinnedMesh::Ptr object) 
  {
    var skeleton = object->skeleton;
    var bones = skeleton.bones;

    if ( capabilities.floatVertexTextures ) {

      return 1024;

    } else {

      // default for when object is not specified
      // ( for example when prebuilding shader to be used with multiple objects )
      //
      //  - leave some extra space for other uniforms
      //  - limit here is ANGLE's 254 max uniform vectors
      //    (up to 54 should be safe)

      var nVertexUniforms = capabilities.maxVertexUniforms;
      var nVertexMatrices = Math.floor( ( nVertexUniforms - 20 ) / 4 );

      var maxBones = Math.min( nVertexMatrices, bones.length );

      if ( maxBones < bones.length ) {

        console.warn( 'THREE.WebGLRenderer: Skeleton has ' + bones.length + ' bones. This GPU supports ' + maxBones + '.' );
        return 0;

      }

      return maxBones;

    }

  }
  
};

}
}
#endif //THREE_QT_PROGRAMS_H
