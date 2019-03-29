//
// Created by byter on 3/29/19.
//

#ifndef THREE_PP_MESHDISTANCEMATERIAL_H
#define THREE_PP_MESHDISTANCEMATERIAL_H

#include <QColor>
#include <threepp/material/MeshDistanceMaterial.h>

#include "Material.h"

namespace three {
namespace quick {


class MeshDistanceMaterial : public Material,
                             public Diffuse<MeshDistanceMaterial>,
                             public AlphaMap<MeshDistanceMaterial>,
                             public DisplacementMap<MeshDistanceMaterial>
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(three::quick::Texture *map READ map WRITE setMap NOTIFY mapChanged)
  Q_PROPERTY(three::quick::Texture *alphaMap READ alphaMap WRITE setAlphaMap NOTIFY alphaMapChanged)
  Q_PROPERTY(three::quick::Texture *displacementMap READ displacementMap WRITE setDisplacementMap NOTIFY displacementMapChanged)
  Q_PROPERTY(float displacementScale READ displacementScale WRITE setDisplacementScale NOTIFY displacementScaleChanged)
  Q_PROPERTY(float displacementBias READ displacementBias WRITE setDisplacementBias NOTIFY displacementBiasChanged)

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  three::MeshDistanceMaterial::Ptr _material;

  MeshDistanceMaterial(three::MeshDistanceMaterial::Ptr mat, QObject *parent=nullptr)
  : Material(material::Typer(this), parent), _material(mat) {}

  MeshDistanceMaterial(QObject *parent=nullptr)
  : Material(material::Typer(this), parent) {}

  void applyColor(const QColor &color)
  {
    setColor(color);
  }

  void setAndConfigure(three::Material::Ptr material) override
  {
    _material = std::dynamic_pointer_cast<three::MeshDistanceMaterial>(material);
    if (!_material) {
      qCritical() << "MaterialHandler: received incompatible material";
    }
    Material::setAndConfigure(material);
    applyDiffuse(_material);
    applyAlphaMap(_material);
    applyDisplacementMap(_material);
  }

  three::MeshDistanceMaterial::Ptr createMaterial()
  {
    auto material = three::MeshDistanceMaterial::make();

    setBaseProperties(material);
    applyDiffuse(material);
    applyAlphaMap(material);
    applyDisplacementMap(material);

    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshDistanceMaterial::Ptr(_material->cloned()) : createMaterial();
    return new MeshDistanceMaterial(material);
  }
  
signals:
  void colorChanged();
  void opacityChanged();
  void mapChanged();
  void alphaMapChanged();
  void displacementMapChanged();
  void displacementScaleChanged();
  void displacementBiasChanged();
};

}
}

#endif //THREE_PP_MESHDISTANCEMATERIAL_H
