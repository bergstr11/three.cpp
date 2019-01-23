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

class MeshBasicMaterial : public Material, public Diffuse<MeshBasicMaterial>
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(three::quick::Texture *map READ map WRITE setMap NOTIFY mapChanged)

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  three::MeshBasicMaterial::Ptr _material;

  MeshBasicMaterial(three::MeshBasicMaterial::Ptr mat, QObject *parent=nullptr)
  : Material(material::Typer(this), parent), Diffuse(this), _material(mat) {}

  MeshBasicMaterial(QObject *parent=nullptr)
  : Material(material::Typer(this), parent), Diffuse(this) {}

  void applyColor(const QColor &color)
  {
    setColor(color);
  }

  void setAndConfigureObject(three::Material::Ptr material) override
  {
    _material = std::dynamic_pointer_cast<three::MeshBasicMaterial>(material);
    if (!_material) {
      qCritical() << "MaterialHandler: received incompatible material";
    }
    Material::setAndConfigureObject(material);
    applyDiffuse(_material);
  }

  three::MeshBasicMaterial::Ptr createMaterial()
  {
    _material = three::MeshBasicMaterial::make();

    setBaseProperties(_material);
    applyDiffuse(_material);

    return _material;
  }

  three::Material::Ptr getMaterial() override
  {
    return _material ? _material : createMaterial();
  }

signals:
  void colorChanged();
  void opacityChanged();
  void mapChanged();
};

}
}

#endif //THREEPPQ_MESHBASICMATERIAL_H
