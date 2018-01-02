//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MeshLambertMaterial_H
#define THREEPP_MeshLambertMaterial_H

#include <QColor>
#include <material/MeshLambertMaterial.h>
#include "../textures/Texture.h"
#include "Material.h"

namespace three {
namespace quick {

class MeshLambertMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)

  QColor _color;
  Texture *_envMap = nullptr;

  three::MeshLambertMaterial::Ptr _material;

public:
  QColor color() const {return _color;}
  Texture *envMap() const {return _envMap;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }
  void setEnvMap(Texture *envMap) {
    if(_envMap != envMap) {
      _envMap = envMap;
      emit envMapChanged();
    }
  }

  three::MeshLambertMaterial::Ptr getMaterial()
  {
    if(!_material) {
      _material = three::MeshLambertMaterial::make();
      if(_color.isValid())
        _material->color = Color(_color.redF(), _color.greenF(), _color.blueF());

      _material->wireframe = _wireframe;

      if(_envMap) {
        _material->envMap = _envMap->getCubeTexture();
        if(!_material->envMap)
          qWarning() << "envMap set to non-cube texture is ignored";
      }
    }
    return _material;
  }

  void addTo(ObjectRootContainer *container) override
  {
    container->addMaterial(this);
  }

  void identify(MeshCreator *creator) override
  {
    creator->material(getMaterial());
  }

signals:
  void colorChanged();
  void envMapChanged();
};

}
}

#endif //THREEPP_MeshLambertMaterial_H
