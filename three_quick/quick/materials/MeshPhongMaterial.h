//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MESHPHONGMATERIAL_H
#define THREEPP_MESHPHONGMATERIAL_H

#include <QColor>
#include <material/MeshPhongMaterial.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshPhongMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(bool dithering READ dithering WRITE setDithering NOTIFY ditheringChanged)

  QColor _color;
  bool _dithering = false;

  three::MeshPhongMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  bool dithering() const {return _dithering;}

  void setDithering(bool dithering) {
    if(_dithering != dithering) {
      _dithering = dithering;
      emit ditheringChanged();
    }
  }

  three::MeshPhongMaterial::Ptr getMaterial()
  {
    if(!_material) {
      _material = three::MeshPhongMaterial::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _dithering);
      _material->wireframe = _wireframe;
    }
    return _material;
  }

  void addTo(ObjectRootContainer *container) override
  {
    container->addMaterial(this);
  }

  void identify(MeshCreator &creator) override
  {
    creator.material(getMaterial());
  }

signals:
  void colorChanged();
  void ditheringChanged();
};

}
}

#endif //THREEPP_MESHPHONGMATERIAL_H
