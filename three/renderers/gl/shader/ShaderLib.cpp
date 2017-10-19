//
// Created by byter on 18.10.17.
//

#include "ShaderLib.h"

static void qInitResource()
{
  static bool done = false;

  if(done) return;
  done = true;

  //must do this outside of namespace - hence this function
  Q_INIT_RESOURCE(ShaderLib);
  Q_INIT_RESOURCE(ShaderChunk);
}

namespace three {
namespace gl {

void ShaderLib::init() {
  qInitResource();
}

}
}
