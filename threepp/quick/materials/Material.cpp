//
// Created by byter on 12/30/17.
//

#include "ShaderMaterial.h"
#include "MeshPhongMaterial.h"
#include "MeshLambertMaterial.h"
#include "MeshBasicMaterial.h"

namespace three {
namespace quick {

void Material::setMap(Texture *map)
{
  if(_map != map) {
    _map = map;
    if(material()) {
      material()->map = _map->getTexture();
      material()->needsUpdate = true;
    }
    emit mapChanged();
  }
}

void Material::setBaseProperties(three::Material::Ptr material)
{
  material->wireframe = _wireframe;
  material->flatShading = _flatShading;
  material->visible = _visible;
  if(_map) material->map = _map->getTexture();
}

three::ShaderMaterial::Ptr ShaderMaterial::createMaterial() const
{
  three::Side side = (three::Side)_side;

  gl::ShaderID shaderId = gl::toShaderID(_shaderID.toStdString());
  gl::ShaderInfo shaderInfo = gl::shaderlib::get(shaderId);
  auto material = three::ShaderMaterial::make(shaderInfo.uniforms,
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
        material->uniforms.set(name, var.toBool());
        break;
      case QVariant::Int:
        material->uniforms.set(name, var.toInt());
        break;
      case QVariant::UInt:
        material->uniforms.set(name, var.toUInt());
        break;
      case QVariant::Double:
        material->uniforms.set(name, (float)var.toDouble());
        break;
      case QMetaType::QObjectStar: {
        Texture *texture = var.value<Texture *>();
        if(texture) {
          texture->setUniform(material->uniforms, name);
        }
        break;
      }
    }
  }
  return material;
}

void MeshPhongMaterial::addTo(ObjectRootContainer *container)
{
  container->addMaterial(this);
}

void MeshLambertMaterial::addTo(ObjectRootContainer *container)
{
  container->addMaterial(this);
}

void MeshBasicMaterial::addTo(ObjectRootContainer *container)
{
  container->addMaterial(this);
}

}
}
