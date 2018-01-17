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

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshLambertMaterial(three::MeshLambertMaterial::Ptr mat, QObject *parent)
     : Material(parent), _material(mat) {}

  MeshLambertMaterial(QObject *parent=nullptr)
     : Material(parent) {}

  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  Texture *envMap() const {return _envMap;}

  void setEnvMap(Texture *envMap) {
    if(_envMap != envMap) {
      _envMap = envMap;
      if(_material) {
        _material->envMap = _envMap->getTexture();
        _material->needsUpdate = true;
      }
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
      _material->flatShading = _flatShading;

      if(_envMap) {
        _material->envMap = _envMap->getTexture();
        if(!_material->envMap)
          qWarning() << "envMap set to non-cube texture is ignored";
      }
    }
    return _material;
  }

  void addTo(ObjectRootContainer *container) override;

  void identify(MeshCreator &creator) override
  {
    creator.material(getMaterial());
  }

signals:
  void colorChanged();
  void envMapChanged();
};

}
}

#endif //THREEPP_MeshLambertMaterial_H
