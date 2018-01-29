//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MESHBASICMATERIAL_H
#define THREEPP_MESHBASICMATERIAL_H

#include <QColor>
#include <material/MeshBasicMaterial.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshBasicMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

  QColor _color;
  float _opacity = 1.0f;

  three::MeshBasicMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshBasicMaterial(three::MeshBasicMaterial::Ptr mat, QObject *parent)
  : Material(parent), _material(mat) {}

  MeshBasicMaterial(QObject *parent=nullptr)
  : Material(parent) {}

  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  float opacity() const {return _opacity;}

  void setOpacity(float opacity) {
    if(_opacity != opacity) {
      _opacity = opacity;
      emit opacityChanged();
    }
  }

  three::MeshBasicMaterial::Ptr createMaterial()
  {
    _material = three::MeshBasicMaterial::make();
    _material->color = Color(_color.redF(), _color.greenF(), _color.blueF());
    _material->opacity = _opacity;
    if(_opacity < 1.0f) _material->transparent = true;

    setBaseProperties(_material);

    return _material;
  }

  void addTo(ObjectRootContainer *container) override;

  void identify(MeshCreator &creator) override
  {
    creator.material(createMaterial());
  }

signals:
  void colorChanged();
  void opacityChanged();
};

}
}

#endif //THREEPP_MESHBASICMATERIAL_H
