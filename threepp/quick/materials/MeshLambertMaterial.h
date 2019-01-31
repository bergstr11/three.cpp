//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_MeshLambertMaterial_H
#define THREEPPQ_MeshLambertMaterial_H

#include <QColor>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/quick/textures/Texture.h>
#include "Material.h"

namespace three {
namespace quick {

class MeshLambertMaterial : public Material,
   public Diffuse<MeshLambertMaterial>,
   public Emissive<MeshLambertMaterial>,
   public EnvMap<MeshLambertMaterial>
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(three::quick::Texture *map READ map WRITE setMap NOTIFY mapChanged)
  Q_PROPERTY(QColor emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  three::MeshLambertMaterial::Ptr _material;

  MeshLambertMaterial(three::MeshLambertMaterial::Ptr mat, QObject *parent=nullptr)
     : Material(material::Typer(this), parent), _material(mat) {}

  MeshLambertMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent) {}

  void applyColor(const QColor &color)
  {
    setColor(color);
  }

  void setAndConfigure(three::Material::Ptr material) override
  {
    _material = std::dynamic_pointer_cast<three::MeshLambertMaterial>(material);
    if (!_material) {
      qCritical() << "MaterialHandler: received incompatible material";
    }
    Material::setAndConfigure(material);
    applyDiffuse(_material);
    applyEmissive(_material);
    applyEnvMap(_material);
  }

  three::MeshLambertMaterial::Ptr createMaterial()
  {
    auto material = three::MeshLambertMaterial::make();

    setBaseProperties(material);
    applyDiffuse(material);
    applyEmissive(material);
    applyEnvMap(material);

    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshLambertMaterial::Ptr(_material->cloned()) : createMaterial();
    return new MeshLambertMaterial(material);
  }

signals:
  void colorChanged();
  void opacityChanged();
  void mapChanged();
  void emissiveChanged();
  void envMapChanged();
};

}
}

#endif //THREEPPQ_MeshLambertMaterial_H
