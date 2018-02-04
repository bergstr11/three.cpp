//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MESHPHONGMATERIAL_H
#define THREEPP_MESHPHONGMATERIAL_H

#include <QColor>
#include <three/material/MeshPhongMaterial.h>
#include <three/quick/textures/Texture.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshPhongMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(bool dithering READ dithering WRITE setDithering NOTIFY ditheringChanged)
  Q_PROPERTY(Texture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)

  QColor _color;
  float _opacity = 1.0f;
  bool _dithering = false;

  Texture *_normalMap = nullptr;

  three::MeshPhongMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshPhongMaterial(three::MeshPhongMaterial::Ptr mat, QObject *parent)
     : Material(parent), _material(mat) {}

  MeshPhongMaterial(QObject *parent=nullptr)
     : Material(parent) {}

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

  float opacity() const {return _opacity;}

  void setOpacity(float opacity) {
    if(_opacity != opacity) {
      _opacity = opacity;
      emit opacityChanged();
    }
  }

  Texture *normalMap() const {return _normalMap;}

  void setNormalMap(Texture *normalMap) {
    if(_normalMap != normalMap) {
      _normalMap = normalMap;
      if(_material) {
        _material->normalMap = _normalMap->getTexture();
        _material->needsUpdate = true;
      }
      emit normalMapChanged();
    }
  }

  three::MeshPhongMaterial::Ptr createMaterial()
  {
    _material = three::MeshPhongMaterial::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _dithering);
    _material->opacity = _opacity;
    if(_opacity < 1.0f) _material->transparent = true;
    if(_normalMap) _material->normalMap = _normalMap->getTexture();

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
  void ditheringChanged();
  void normalMapChanged();
  void opacityChanged();
};

}
}

#endif //THREEPP_MESHPHONGMATERIAL_H
