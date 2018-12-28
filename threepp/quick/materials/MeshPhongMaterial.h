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
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
  Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
  Q_PROPERTY(bool dithering READ dithering WRITE setDithering NOTIFY ditheringChanged)
  Q_PROPERTY(three::quick::Texture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)
  Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
  Q_PROPERTY(float refractionRatio READ refractionRatio WRITE setRefractionRatio NOTIFY refractionRatioChanged)

  TrackingProperty<QColor> _color {0x111111};
  TrackingProperty<float> _opacity {1.0f};
  TrackingProperty<QColor> _specular {0x111111};
  TrackingProperty<float> _shininess {30};
  TrackingProperty<float> _reflectivity {1};
  TrackingProperty<bool> _dithering {false};
  TrackingProperty<float> _refractionRatio {0.98f};

  Texture *_normalMap = nullptr;
  TrackingProperty<Texture *> _envMap {nullptr};

  three::MeshPhongMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshPhongMaterial(three::MeshPhongMaterial::Ptr mat, QObject *parent = nullptr)
     : Material(material::Typer(this), parent), _material(mat),
       _color(QColor(mat->color.r, mat->color.g, mat->color.b)),
       _specular(QColor(mat->specular.r, mat->specular.g, mat->specular.b)),
       _opacity(mat->opacity),
       _shininess(mat->shininess),
       _reflectivity(mat->reflectivity),
       _dithering(mat->dithering),
       _refractionRatio(mat->refractionRatio)
  {}

  MeshPhongMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent) {}

  QColor color() const {return _color;}

  void setColor(const QColor &color) override {
    if(_color != color) {
      _color = color;
      if(_material) {
        _material->color.set(_color().redF(), _color().greenF(), _color().blueF());
        _material->needsUpdate = true;
      }
      emit colorChanged();
    }
  }

  QColor specular() const {return _specular;}

  void setSpecular(const QColor &specular) {
    if(_specular != specular) {
      _specular = specular;
      if(_material) {
        _material->specular.set(_specular().redF(), _specular().greenF(), _specular().blueF());
        _material->needsUpdate = true;
      }
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

  float reflectivity() const {return _reflectivity;}

  void setReflectivity(float reflectivity) {
    if(_reflectivity != reflectivity) {
      _reflectivity = reflectivity;
      if(_material) {
        _material->reflectivity = _reflectivity;
        _material->needsUpdate = true;
      }
      emit reflectivityChanged();
    }
  }

  float refractionRatio() const {return _refractionRatio;}

  void setRefractionRatio(float refractionRatio) {
    if(_refractionRatio != refractionRatio) {
      _refractionRatio = refractionRatio;
      if(_material) {
        _material->refractionRatio = _refractionRatio;
        _material->needsUpdate = true;
      }
      emit refractionRatioChanged();
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
    _material = three::MeshPhongMaterial::make(Color(_color().redF(), _color().greenF(), _color().blueF()), _dithering);
    _material->opacity = _opacity;
    _material->specular.set(_specular().redF(), _specular().greenF(), _specular().blueF());
    _material->refractionRatio = _refractionRatio;
    _material->shininess = _shininess;
    _material->reflectivity = _reflectivity;
    _material->dithering = _dithering;

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
  void reflectivityChanged();
  void refractionRatioChanged();
};

}
}

#endif //THREEPPQ_MESHPHONGMATERIAL_H
