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
  Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
  Q_PROPERTY(bool dithering READ dithering WRITE setDithering NOTIFY ditheringChanged)
  Q_PROPERTY(three::quick::Texture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)

  QColor _color {255, 255, 255}, _specular {0x111111};
  float _opacity = 1.0f;
  float _shininess = 30;
  bool _dithering = false;

  Texture *_normalMap = nullptr;
  TrackingProperty<Texture *> _envMap {nullptr};

  three::MeshPhongMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshPhongMaterial(three::MeshPhongMaterial::Ptr mat, QObject *parent=nullptr)
     : Material(material::Typer(this), parent), _material(mat) {}

  MeshPhongMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent) {}

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
      if(_material) {
        _material->dithering = _dithering;
        _material->needsUpdate = true;
      }
      emit ditheringChanged();
    }
  }

  float opacity() const {return _opacity;}

  void setOpacity(float opacity) {
    if(_opacity != opacity) {
      _opacity = opacity;
      if(_material) {
        _material->opacity = _opacity;
        _material->needsUpdate = true;
      }
      emit opacityChanged();
    }
  }

  float shininess() const {return _shininess;}

  void setShininess(float shininess) {
    if(_shininess != shininess) {
      _shininess = shininess;
      if(_material) {
        _material->shininess = _shininess;
        _material->needsUpdate = true;
      }
      emit shininessChanged();
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

  Texture *envMap() const {return _envMap;}

  void setEnvMap(Texture *envMap) {
    if(_envMap != envMap) {
      _envMap = envMap;
      if(_material) {
        _material->envMap = _envMap ? _envMap().getTexture() : nullptr;
        _material->needsUpdate = true;
      }
      emit envMapChanged();
    }
  }

  three::MeshPhongMaterial::Ptr createMaterial()
  {
    _material = three::MeshPhongMaterial::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _dithering);
    _material->opacity = _opacity;
    _material->specular = Color(_specular.redF(), _specular.greenF(), _specular.blueF());
    if(_normalMap) _material->normalMap = _normalMap->getTexture();

    setBaseProperties(_material);

    return _material;
  }

  three::Material::Ptr getMaterial() override
  {
    return _material ? _material : createMaterial();
  }

signals:
  void colorChanged();
  void specularChanged();
  void ditheringChanged();
  void normalMapChanged();
  void opacityChanged();
  void envMapChanged();
  void shininessChanged();
};

}
}

#endif //THREEPPQ_MESHPHONGMATERIAL_H
