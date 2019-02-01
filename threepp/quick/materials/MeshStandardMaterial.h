//
// Created by byter on 5/23/18.
//

#ifndef THREE_PP_MESHSTANDARDMATERIAL_H
#define THREE_PP_MESHSTANDARDMATERIAL_H

#include <QColor>
#include <threepp/material/MeshStandardMaterial.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshStandardMaterial : public Material,
   public Diffuse<MeshStandardMaterial>,
   public Emissive<MeshStandardMaterial>,
   public EnvMap<MeshStandardMaterial>,
   public LightMap<MeshStandardMaterial>,
   public NormalMap<MeshStandardMaterial>,
   public BumpMap<MeshStandardMaterial>,
   public RoughnessMap<MeshStandardMaterial>,
   public MetalnessMap<MeshStandardMaterial>
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(three::quick::Texture *map READ map WRITE setMap NOTIFY mapChanged)
  Q_PROPERTY(float metalness READ metalness WRITE setMetalness NOTIFY metalnessChanged)
  Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
  Q_PROPERTY(QColor emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
  Q_PROPERTY(float emissiveIntensity READ emissiveIntensity WRITE setEmissiveIntensity NOTIFY emissiveIntensityChanged)
  Q_PROPERTY(Texture *emissiveMap READ emissiveMap WRITE setEmissiveMap NOTIFY emissiveMapChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)
  Q_PROPERTY(Texture *lightMap READ lightMap WRITE setLightMap NOTIFY lightMapChanged)
  Q_PROPERTY(float lightMapIntensity READ lightMapIntensity WRITE setLightMapIntensity NOTIFY lightMapIntensityChanged)
  Q_PROPERTY(Texture *metalnessMap READ metalnessMap WRITE setMetalnessMap NOTIFY metalnessMapChanged)
  Q_PROPERTY(Texture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
  Q_PROPERTY(float roughness READ roughness WRITE setRoughness NOTIFY roughnessChanged)
  Q_PROPERTY(Texture *roughnessMap READ roughnessMap WRITE setRoughnessMap NOTIFY roughnessMapChanged)
  Q_PROPERTY(float refractionRatio READ refractionRatio WRITE setRefractionRatio NOTIFY refractionRatioChanged)
  Q_PROPERTY(Texture *bumpMap READ bumpMap WRITE setBumpMap NOTIFY bumpMapChanged)
  Q_PROPERTY(float bumpScale READ bumpScale WRITE setBumpScale NOTIFY bumpScaleChanged)

  TrackingProperty<float> _metalness {0.5f};
  TrackingProperty<Texture *> _metalnessMap {nullptr};

protected:
  three::Material::Ptr material() const override {return _material;}

  void setProperties(three::MeshStandardMaterial::Ptr material)
  {
    setBaseProperties(material);
    applyDiffuse(material);
    applyEmissive(material);
    applyEnvMap(material);
    applyLightMap(material);
    applyNormalMap(material);
    applyBumpMap(material);
    applyRoughnessMap(material);
    applyMetalnessMap(material);
  }

  MeshStandardMaterial(three::MeshStandardMaterial::Ptr mat, const material::Typer &typer, QObject *parent = nullptr)
     : Material(typer, parent), _material(mat)
  {}

  MeshStandardMaterial(const material::Typer &typer, QObject *parent=nullptr)
     : Material(typer, parent) {}

public:
  three::MeshStandardMaterial::Ptr _material;

  MeshStandardMaterial(three::MeshStandardMaterial::Ptr mat, QObject *parent = nullptr)
     : Material(material::Typer(this), parent), _material(mat)
  {}

  MeshStandardMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent) {}

  void applyColor(const QColor &color)
  {
    setColor(color);
  }

  void setAndConfigure(three::Material::Ptr material) override
  {
    _material = std::dynamic_pointer_cast<three::MeshStandardMaterial>(material);
    if(!_material) {
      qCritical() << "MaterialHandler: received incompatible material";
    }
    Material::setAndConfigure(material);
    applyDiffuse(_material);
    applyEnvMap(_material);
    applyEmissive(_material);
    applyLightMap(_material);
    applyNormalMap(_material);
    applyBumpMap(_material);
    applyRoughnessMap(_material);
    applyMetalnessMap(_material);

    if(_metalness.isSet()) _material->metalness = _metalness;
    if(_metalnessMap.isSet()) _material->metalnessMap = _metalnessMap().getTexture();
  }

  three::MeshStandardMaterial::Ptr createMeshStandardMaterial()
  {
    auto material = three::MeshStandardMaterial::make();
    setProperties(material);
    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMeshStandardMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshStandardMaterial::Ptr(_material->cloned()) : createMeshStandardMaterial();
    return new MeshStandardMaterial(material);
  }

signals:
  void emissiveChanged();
  void emissiveIntensityChanged();
  void emissiveMapChanged();
  void colorChanged();
  void opacityChanged();
  void mapChanged();
  void envMapChanged();
  void lightMapChanged();
  void lightMapIntensityChanged();
  void metalnessChanged();
  void roughnessChanged();
  void reflectivityChanged();
  void refractionRatioChanged();
  void bumpMapChanged();
  void bumpScaleChanged();
  void metalnessMapChanged();
  void normalMapChanged();
  void roughnessMapChanged();
};

}
}

#endif //THREE_PP_MESHSTANDARDMATERIAL_H
