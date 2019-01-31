//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_MESHBASICMATERIAL_H
#define THREEPPQ_MESHBASICMATERIAL_H

#include <QColor>
#include <threepp/material/MeshBasicMaterial.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshBasicMaterial : public Material,
                          public Diffuse<MeshBasicMaterial>,
                          public AoMap<MeshBasicMaterial>,
                          public EnvMap<MeshBasicMaterial>,
                          public AlphaMap<MeshBasicMaterial>,
                          public SpecularMap<MeshBasicMaterial>,
                          public LightMap<MeshBasicMaterial>
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(three::quick::Texture *map READ map WRITE setMap NOTIFY mapChanged)
  Q_PROPERTY(three::quick::Texture *aoMap READ aoMap WRITE setAoMap NOTIFY aoMapChanged)
  Q_PROPERTY(float aoMapIntensity READ aoMapIntensity WRITE setAoMapIntensity NOTIFY aoMapIntensityChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)
  Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
  Q_PROPERTY(float refractionRatio READ refractionRatio WRITE setRefractionRatio NOTIFY refractionRatioChanged)
  Q_PROPERTY(float envMapIntensity READ envMapIntensity WRITE setEnvMapIntensity NOTIFY envMapIntensityChanged)
  Q_PROPERTY(Texture *alphaMap READ alphaMap WRITE setAlphaMap NOTIFY alphaMapChanged)

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  three::MeshBasicMaterial::Ptr _material;

  MeshBasicMaterial(three::MeshBasicMaterial::Ptr mat, QObject *parent=nullptr)
  : Material(material::Typer(this), parent), _material(mat) {}

  MeshBasicMaterial(QObject *parent=nullptr)
  : Material(material::Typer(this), parent) {}

  void applyColor(const QColor &color)
  {
    setColor(color);
  }

  void setAndConfigure(three::Material::Ptr material) override
  {
    _material = std::dynamic_pointer_cast<three::MeshBasicMaterial>(material);
    if (!_material) {
      qCritical() << "MaterialHandler: received incompatible material";
    }
    Material::setAndConfigure(material);
    applyDiffuse(_material);
    applyAoMap(_material);
    applyEnvMap(_material);
    applyAlphaMap(_material);
    applySpecularMap(_material);
    applyLightMap(_material);
  }

  three::MeshBasicMaterial::Ptr createMaterial()
  {
    auto material = three::MeshBasicMaterial::make();

    setBaseProperties(material);
    applyDiffuse(material);
    applyAoMap(material);
    applyEnvMap(material);
    applyAlphaMap(material);
    applySpecularMap(material);
    applyLightMap(material);

    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshBasicMaterial::Ptr(_material->cloned()) : createMaterial();
    return new MeshBasicMaterial(material);
  }

signals:
  void colorChanged();
  void opacityChanged();
  void mapChanged();
  void aoMapChanged();
  void aoMapIntensityChanged();
  void envMapChanged();
  void reflectivityChanged();
  void refractionRatioChanged();
  void envMapIntensityChanged();
  void alphaMapChanged();
};

}
}

#endif //THREEPPQ_MESHBASICMATERIAL_H
