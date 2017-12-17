//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MeshLambertMaterial_H
#define THREEPP_MeshLambertMaterial_H

#include <QColor>
#include <material/MeshLambertMaterial.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshLambertMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

  QColor _color;

public:
  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  three::Material::Ptr create() override
  {
    three::MeshLambertMaterial::Ptr material = three::MeshLambertMaterial::make();
    material->color = Color(_color.redF(), _color.greenF(), _color.blueF());
    material->wireframe = _wireframe;
    return material;
  }

signals:
  void colorChanged();
};

}
}

#endif //THREEPP_MeshLambertMaterial_H
