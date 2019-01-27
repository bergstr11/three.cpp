//
// Created by byter on 12/30/17.
//

#include "ShaderMaterial.h"
#include "MeshPhongMaterial.h"
#include "MeshLambertMaterial.h"
#include "MeshBasicMaterial.h"
#include "MeshStandardMaterial.h"
#include <QVector3D>
#include <QTextStream>

namespace three {
namespace quick {

void Material::setBaseProperties(three::Material::Ptr material)
{
  material->name = _name().toStdString();
  material->wireframe = _wireframe;
  material->flatShading = _flatShading;
  material->visible = _visible;
  material->dithering = _dithering;
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

three::ShaderMaterial::Ptr ShaderMaterial::createMaterial()
{
  three::Side side = (three::Side)_side;

  three::ShaderMaterial::Ptr mat;
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
    mat = material;
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
    mat = material;
  }
  setBaseProperties(mat);
  return mat;
}

QString texture(three::Texture::Ptr textr)
{
  if(!textr) {
    return "no";
  }
  else if(three::ImageTexture *tex = textr->typer) {
    return "Image";
  }
  else if(three::DepthTexture *tex = textr->typer) {
    return "Depth";
  }
  if(three::DataTexture *tex = textr->typer) {
    return "Data";
  }
  return "yes";
}

QString Material::getInfo()
{
  QString info;
  QTextStream stream (&info, QIODevice::WriteOnly);
  const three::Material &m = *material();
  three::material::Diffuse *df = nullptr;
  three::material::LightMap *lm = nullptr;
  three::material::Emissive *em = nullptr;
  three::material::AoMap *ao = nullptr;
  three::material::EnvMap *ev = nullptr;
  three::material::AlphaMap *am = nullptr;
  three::material::SpecularMap *sm = nullptr;
  three::material::Specular *s = nullptr;
  three::material::DisplacementMap *dm = nullptr;
  three::material::BumpMap *bm = nullptr;
  three::material::NormalMap *nm = nullptr;
  three::material::RoughnessMap *rm = nullptr;
  three::material::MetalnessMap *mm = nullptr;

  if(three::MeshPhongMaterial *mat = m.typer) {
    stream << "MeshPhongMaterial\n";

    df = mat;
    lm = mat;
    ao = mat;
    em = mat;
    bm = mat;
    nm = mat;
    dm = mat;
    s = mat;
    am = mat;
    ev = mat;
  }
  else if(three::MeshLambertMaterial *mat = m.typer) {
    stream << "MeshLambertMaterial\n";

    df = mat;
    lm = mat;
    ao = mat;
    em = mat;
    am = mat;
    ev = mat;
    sm = mat;
  }
  else if(three::MeshBasicMaterial *mat = m.typer) {
    stream << "MeshBasicMaterial\n";

    df = mat;
    lm = mat;
    ao = mat;
    am = mat;
    ev = mat;
    sm = mat;
  }
  if(three::MeshStandardMaterial *mat = m.typer) {
    stream << "MeshStandardMaterial\n";

    df = mat;
    lm = mat;
    ao = mat;
    em = mat;
    bm = mat;
    nm = mat;
    dm = mat;
    am = mat;
    ev = mat;
    rm = mat;
    mm = mat;
  }
  stream << "id:\t\t" << m.id;
  stream << "\nname:\t\t" << m.name.c_str();
  stream << "\nfog:\t\t" << m.fog;
  stream << "\nlights:\t\t" << m.lights;
  stream << "\nambientColor:\t" << m.ambientColor.r << ":" << m.ambientColor.g << ":" << m.ambientColor.b;
  stream << "\nblending:\t\t" << (int)m.blending;
  stream << "\nside:\t\t" << (int)m.side;
  stream << "\nflatShading:\t\t" << m.flatShading;
  stream << "\nvertexColors:\t" << (int)m.vertexColors;
  stream << "\nblendSrc:\t\t" << (int)m.blendSrc;
  stream << "\nblendDst:\t\t" << (int)m.blendDst;
  stream << "\nblendEquation:\t" << (int)m.blendEquation;
  stream << "\nblendSrcAlpha:\t" << (int)m.blendSrcAlpha;
  stream << "\nblendDstAlpha:\t" << (int)m.blendDstAlpha;
  stream << "\nblendEquationAlpha:\t" << (int)m.blendEquationAlpha;
  stream << "\ndepthFunc:\t\t" << (int)m.depthFunc;
  stream << "\ndepthTest:\t\t" << m.depthTest;
  stream << "\ndepthWrite:\t\t" << m.depthWrite;
  stream << "\nclippingPlanes:\t" << m.clippingPlanes.size();
  stream << "\ndepthWrite:\t\t" << m.clipIntersection;
  stream << "\nclipIntersection:\t" << m.clipShadows;
  stream << "\ncolorWrite:\t\t" << m.colorWrite;
  stream << "\nwireframe:\t\t" << m.wireframe;
  stream << "\nwireframeLineWidth:\t" << m.wireframeLineWidth;
  stream << "\nwireframeLineCap:\t" << (int)m.wireframeLineCap;
  stream << "\nwireframeLineJoin:\t" << (int)m.wireframeLineJoin;
  stream << "\nprecision:\t\t" << (int)m.precision;
  stream << "\npolygonOffset:\t" << m.polygonOffset;
  stream << "\npolygonOffsetFactor:\t" << m.polygonOffsetFactor;
  stream << "\npolygonOffsetUnits:\t" << m.polygonOffsetUnits;
  stream << "\ndithering:\t\t" << m.dithering;
  stream << "\nalphaTest:\t\t" << m.alphaTest;
  stream << "\npremultipliedAlpha:\t" << m.premultipliedAlpha;
  stream << "\nskinning:\t\t" << m.skinning;
  stream << "\nmorphTargets:\t" << m.morphTargets;
  stream << "\nnumSupportedMorphTargets: " << m.numSupportedMorphTargets;
  stream << "\nmorphNormals:\t" << m.morphNormals;
  stream << "\nnumSupportedMorphNormals: " << m.numSupportedMorphNormals;
  stream << "\noverdraw:\t\t" << m.overdraw;

  if(df) {
    stream << "\nDiffuse";
    stream << "\n  color:\t\t" << df->color.r << ":" << df->color.g << ":" << df->color.b;
    stream << "\n  opacity:\t\t" << df->opacity;
    stream << "\n  map:\t\t" << texture(df->map);
  }
  if(lm) {
    stream << "\nLightMap";
    stream << "\n  lightMap:\t\t" << texture(lm->lightMap);
    stream << "\n  lightMapIntensity\t" << lm->lightMapIntensity;
  }
  if(em) {
    stream << "\nEmissive";
    stream << "\n  emissiveMap:\t" << texture(em->emissiveMap);
    stream << "\n  emissive:\t\t" << em->emissive.r << ":" << em->emissive.g << ":" << em->emissive.b;
    stream << "\n  emissiveIntensity:\t" << em->emissiveIntensity;
  }
  if(ao) {
    stream << "\nAoMap";
    stream << "\n  aoMap:\t\t" << texture(ao->aoMap);
    stream << "\n  aoMapIntensity:\t" << ao->aoMapIntensity;
  }
  if(ev) {
    stream << "\nEnvMap";
    stream << "\n  envMap:\t\t" << texture(ev->envMap);
    stream << "\n  combine:\t\t" << (int) ev->combine;
    stream << "\n  reflectivity:\t" << ev->reflectivity;
    stream << "\n  refractionRatio:\t" << ev->refractionRatio;
    stream << "\n  envMapIntensity:\t" << ev->envMapIntensity;
  }
  if(am) {
    stream << "\nAlphaMap";
    stream << "\n  alphaMap:\t\t" << texture(am->alphaMap);
  }
  if(sm) {
    stream << "\nSpecularMap";
    stream << "\n  specularMap:\t\t" << texture(sm->specularMap);
  }
  if(s) {
    stream << "\nSpecular";
    stream << "\n  specularMap:\t" << texture(s->specularMap);
    stream << "\n  shininess:\t\t" << s->shininess;
    stream << "\n  specular:\t\t" << s->specular.r << ":" << s->specular.g << ":" << s->specular.b;
  }
  if(sm) {
    stream << "\nDisplacementMap";
    stream << "\n  displacementMap:\t\t" << texture(dm->displacementMap);
    stream << "\n  displacementScale:\t\t" << dm->displacementScale;
    stream << "\n  displacementBias:\t\t" << dm->displacementBias;
  }
  if(bm) {
    stream << "\nBumpMap";
    stream << "\n  bumpMap:\t\t" << texture(bm->bumpMap);
    stream << "\n  bumpScale:\t" << bm->bumpScale;
  }
  if(nm) {
    stream << "\nNormalMap";
    stream << "\n  normalMap:\t" << texture(nm->normalMap);
    stream << "\n  normalScale:\t" << nm->normalScale.x() << nm->normalScale.y();
  }
  if(rm) {
    stream << "\nRoughnessMap";
    stream << "\n  roughness:\t\t" << rm->roughness;
    stream << "\n  roughnessMap:\t\t" << texture(rm->roughnessMap);
  }
  if(mm) {
    stream << "\nMetalnessMap";
    stream << "\n  metalness:\t\t" << mm->metalness;
    stream << "\n  metalnessMap:\t\t" << texture(mm->metalnessMap);
  }
  stream.flush();
  return info;
}

}
}
