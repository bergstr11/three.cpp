//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_MESHPHONGMATERIAL_H
#define THREEPPQ_MESHPHONGMATERIAL_H

#include <QColor>
#include <threepp/material/MeshPhongMaterial.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshPhongMaterial : public Material,
   public Diffuse<MeshPhongMaterial>,
   public Emissive<MeshPhongMaterial>,
   public EnvMap<MeshPhongMaterial>,
   public LightMap<MeshPhongMaterial>,
   public NormalMap<MeshPhongMaterial>
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(three::quick::Texture *map READ map WRITE setMap NOTIFY mapChanged)
  Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
  Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
  Q_PROPERTY(three::quick::Texture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)
  Q_PROPERTY(float envMapIntensity READ envMapIntensity WRITE setEnvMapIntensity NOTIFY envMapIntensityChanged)
  Q_PROPERTY(Texture *lightMap READ lightMap WRITE setLightMap NOTIFY lightMapChanged)
  Q_PROPERTY(float lightMapIntensity READ lightMapIntensity WRITE setLightMapIntensity NOTIFY lightMapIntensityChanged)
  Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
  Q_PROPERTY(float refractionRatio READ refractionRatio WRITE setRefractionRatio NOTIFY refractionRatioChanged)
  Q_PROPERTY(QColor emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
  Q_PROPERTY(float emissiveIntensity READ emissiveIntensity WRITE setEmissiveIntensity NOTIFY emissiveIntensityChanged)
  Q_PROPERTY(Texture *emissiveMap READ emissiveMap WRITE setEmissiveMap NOTIFY emissiveMapChanged)

  TrackingProperty<QColor> _specular {0x111111};
  TrackingProperty<float> _shininess {30};

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  three::MeshPhongMaterial::Ptr _material;

  MeshPhongMaterial(three::MeshPhongMaterial::Ptr mat, QObject *parent = nullptr)
     : Material(material::Typer(this), parent), _material(mat), Diffuse(this), Emissive(this), EnvMap(this), LightMap(this), NormalMap(this)
  {}

  MeshPhongMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent), Diffuse(this), Emissive(this), EnvMap(this), LightMap(this), NormalMap(this) {}

  void applyColor(const QColor &color)
  {
    setColor(color);
  }

  void setAndConfigureObject(three::Material::Ptr material) override
  {
    _material = std::dynamic_pointer_cast<three::MeshPhongMaterial>(material);
    if (!_material) {
      qCritical() << "MaterialHandler: received incompatible material";
    }
    Material::setAndConfigureObject(material);
    applyDiffuse(_material);
    applyEnvMap(_material);
    applyEmissive(_material);
    applyLightMap(_material);
    applyNormalMap(_material);
  }

  QColor specular() const
  {
    return _material ? QColor::fromRgbF(_material->specular.r, _material->specular.g, _material->specular.b) : _specular;
  }

  void setSpecular(const QColor &specular) {
    if(_specular != specular) {
      _specular = specular;
      if(_material) {
        _material->specular.set(_specular().redF(), _specular().greenF(), _specular().blueF());
        _material->needsUpdate = true;
      }
      emit specularChanged();
    }
  }

  float shininess() const {return _material ? _material->shininess : _shininess;}

  void setShininess(float shininess) {
    if(_shininess != shininess) {
      _shininess = shininess;
      if(_material) {
        _material->shininess = _shininess;
        _material->needsUpdate = true;
      }
      emit shininessChanged();
    }
  }

  three::MeshPhongMaterial::Ptr createMaterial()
  {
    auto material = three::MeshPhongMaterial::make(Color(_color().redF(), _color().greenF(), _color().blueF()), _dithering);
    material->specular.set(_specular().redF(), _specular().greenF(), _specular().blueF());
    material->refractionRatio = _refractionRatio;
    material->shininess = _shininess;

    setBaseProperties(material);
    applyDiffuse(material);
    applyEmissive(material);
    applyEnvMap(material);
    applyLightMap(_material);
    applyNormalMap(_material);

    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshPhongMaterial::Ptr(_material->cloned()) : createMaterial();
    return new MeshPhongMaterial(material);
  }

signals:
  void colorChanged();
  void opacityChanged();
  void mapChanged();
  void lightMapChanged();
  void lightMapIntensityChanged();
  void specularChanged();
  void emissiveChanged();
  void emissiveIntensityChanged();
  void emissiveMapChanged();
  void normalMapChanged();
  void envMapChanged();
  void envMapIntensityChanged();
  void shininessChanged();
  void reflectivityChanged();
  void refractionRatioChanged();
};

}
}

#endif //THREEPPQ_MESHPHONGMATERIAL_H
