//
// Created by byter on 12/30/17.
//

#include "ShaderMaterial.h"
#include <quick/textures/Texture.h>

namespace three {
namespace quick {

three::ShaderMaterial::Ptr ShaderMaterial::createMaterial()
{
  three::Side side = (three::Side)_side;

  gl::ShaderID shaderId = gl::toShaderID(_shaderID.toStdString());
  gl::ShaderInfo shaderInfo = gl::shaderlib::get(shaderId);
  _material = three::ShaderMaterial::make(shaderInfo.uniforms,
                                          shaderInfo.vertexShader,
                                          shaderInfo.fragmentShader,
                                          side,
                                          _depthTest,
                                          _depthWrite);
  for(auto key : _uniforms.keys()) {
    gl::UniformName name = gl::toUniformName(key.toStdString());
    QVariant var = _uniforms[key];
    switch(var.type()) {
      case QVariant::Bool:
        _material->uniforms.set(name, var.toBool());
        break;
      case QVariant::Int:
        _material->uniforms.set(name, var.toInt());
        break;
      case QVariant::UInt:
        _material->uniforms.set(name, var.toUInt());
        break;
      case QVariant::Double:
        _material->uniforms.set(name, (float)var.toDouble());
        break;
      case QMetaType::QObjectStar: {
        Texture *texture = var.value<Texture *>();
        if(texture) {
          texture->setUniform(_material->uniforms, name);
        }
        break;
      }
    }
  }
}

}
}