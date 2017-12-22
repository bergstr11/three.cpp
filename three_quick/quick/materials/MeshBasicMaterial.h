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

  QColor _color;

public:
  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  void identify(MeshCreator *creator) override
  {
    three::MeshBasicMaterial::Ptr material = three::MeshBasicMaterial::make();
    material->color = Color(_color.redF(), _color.greenF(), _color.blueF());
    material->wireframe = _wireframe;
    creator->material(material);
  }

signals:
  void colorChanged();
};

}
}

#endif //THREEPP_MESHBASICMATERIAL_H
