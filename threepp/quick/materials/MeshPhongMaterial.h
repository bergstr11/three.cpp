//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_MESHPHONGMATERIAL_H
#define THREEPPQ_MESHPHONGMATERIAL_H

#include <QColor>
#include <threepp/material/MeshPhongMaterial.h>
#include <threepp/quick/textures/Texture.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshPhongMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(bool dithering READ dithering WRITE setDithering NOTIFY ditheringChanged)
  Q_PROPERTY(Texture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)

  QColor _color {255, 255, 255}, _specular {0x111111};
  float _opacity = 1.0f;
  bool _dithering = false;

  Texture *_normalMap = nullptr;

  three::MeshPhongMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshPhongMaterial(three::MeshPhongMaterial::Ptr mat, QObject *parent=nullptr)
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

  QColor specular() const {return _specular;}

  void setSpecular(const QColor &specular) {
    if(_specular != specular) {
      _specular = specular;
      emit specularChanged();
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
    _material->specular = Color(_specular.redF(), _specular.greenF(), _specular.blueF());
    if(_opacity < 1.0f) _material->transparent = true;
    if(_normalMap) _material->normalMap = _normalMap->getTexture();

    setBaseProperties(_material);

    return _material;
  }

  void identify(MeshCreator &creator) override
  {
    creator.material(_material ? _material : createMaterial());
  }

signals:
  void colorChanged();
  void specularChanged();
  void ditheringChanged();
  void normalMapChanged();
  void opacityChanged();
};

}
}

#endif //THREEPPQ_MESHPHONGMATERIAL_H
