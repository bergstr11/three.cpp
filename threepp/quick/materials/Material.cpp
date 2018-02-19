//
// Created by byter on 12/30/17.
//

#include "ShaderMaterial.h"
#include "MeshPhongMaterial.h"
#include "MeshLambertMaterial.h"
#include "MeshBasicMaterial.h"
#include <QVector3D>

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

void set_uniform(gl::UniformName name, const QVariant &var, three::gl::UniformValues &uniforms)
{
  switch(var.type()) {
    case QVariant::Bool:
      uniforms.set(name, var.toBool());
      break;
    case QVariant::Int:
      uniforms.set(name, var.toInt());
      break;
    case QVariant::UInt:
      uniforms.set(name, var.toUInt());
      break;
    case QVariant::Double:
      uniforms.set(name, (float)var.toDouble());
      break;
    case QVariant::Color: {
      const auto &colr = var.value<QColor>();
      uniforms.set(name, Color(colr.redF(), colr.greenF(), colr.blueF()));
      break;
    }
    case QVariant::Vector3D: {
      const auto &vec = var.value<QVector3D>();
      uniforms.set(name, math::Vector3(vec.x(), vec.y(), vec.z()));
      break;
    }
    case QMetaType::QObjectStar: {
      Texture *texture = var.value<Texture *>();
      if(texture) {
        texture->setUniform(uniforms, name);
      }
      break;
    }
  }
}

three::ShaderMaterial::Ptr ShaderMaterial::createMaterial() const
{
  three::Side side = (three::Side)_side;

  if(!_shaderID.isEmpty()) {
    //predefined shader from library
    gl::ShaderID shaderId = gl::toShaderID(_shaderID.toStdString());
    gl::ShaderInfo shaderInfo = gl::shaderlib::get(shaderId);

    auto material = three::ShaderMaterial::make(shaderInfo.uniforms,
                                                shaderInfo.vertexShader,
                                                shaderInfo.fragmentShader,
                                                side,
                                                _depthTest,
                                                _depthWrite);
    for(auto key : _uniforms.keys()) {
      gl::UniformName name = gl::uniformname::get(key.toStdString());
      set_uniform(name, _uniforms[key], material->uniforms);
    }
    return material;
  }
  else {
    //user-defined shader
    auto material = three::ShaderMaterial::make(gl::UniformValues(),
                                                _vertexShader.data(),
                                                _fragmentShader,
                                                side,
                                                _depthTest,
                                                _depthWrite);
    for(auto key : _uniforms.keys()) {
      std::string name = key.toStdString();
      gl::UniformName uname = material->uniforms.registered(name);
      set_uniform(uname, _uniforms[key], material->uniforms);
    }
    return material;
  }
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
