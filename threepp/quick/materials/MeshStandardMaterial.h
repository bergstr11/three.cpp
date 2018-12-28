//
// Created by byter on 5/23/18.
//

#ifndef THREE_PP_MESHSTANDARDMATERIAL_H
#define THREE_PP_MESHSTANDARDMATERIAL_H

#include <QColor>
#include <threepp/material/MeshStandardMaterial.h>

#include "Material.h"

namespace three {
namespace quick {

class MeshStandardMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(float metalness READ metalness WRITE setMetalness NOTIFY metalnessChanged)
  Q_PROPERTY(float roughness READ roughness WRITE setRoughness NOTIFY roughnessChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)
  Q_PROPERTY(Texture *lightMap READ lightMap WRITE setLightMap NOTIFY lightMapChanged)
  Q_PROPERTY(float refractionRatio READ refractionRatio WRITE setRefractionRatio NOTIFY refractionRatioChanged)

  TrackingProperty<QColor> _color {QColor()};
  TrackingProperty<float> _opacity {1.0f};
  TrackingProperty<float> _roughness {0.5f};
  TrackingProperty<float> _metalness {0.5f};
  TrackingProperty<Texture *> _envMap {nullptr};
  TrackingProperty<Texture *> _lightMap {nullptr};

  TrackingProperty<float> _reflectivity {1};
  TrackingProperty<bool> _dithering {false};
  TrackingProperty<float> _refractionRatio {0.98f};

  three::MeshStandardMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshStandardMaterial(three::MeshStandardMaterial::Ptr mat, QObject *parent = nullptr)
     : Material(material::Typer(this), parent), _material(mat),
       _color(QColor(mat->color.r, mat->color.g, mat->color.b)),
       _opacity(mat->opacity),
       _reflectivity(mat->reflectivity),
       _dithering(mat->dithering),
       _refractionRatio(mat->refractionRatio)
  {}

  MeshStandardMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent) {}

  void setAndConfigureObject(three::Material::Ptr material) override
  {
    Material::setAndConfigureObject(material);

    _material = std::dynamic_pointer_cast<three::MeshStandardMaterial>(material);
    if(!_material) {
      qCritical() << "MaterialHandler: received incompatible material. Double handled?";
    }
    if(_color.isSet()) _material->color.set(_color().redF(), _color().greenF(), _color().blueF());
    if(_opacity.isSet()) _material->opacity = _opacity;
    if(_roughness.isSet()) _material->roughness = _roughness;
    if(_metalness.isSet()) _material->metalness = _metalness;
    if(_envMap.isSet()) _material->envMap = _envMap().getTexture();
    if(_lightMap.isSet()) _material->lightMap = _lightMap().getTexture();
  }

  QColor color() const {return _color;}

  void setColor(const QColor &color) override {
    if(_color != color) {
      _color = color;
      if(_material) {
        _material->color.set(color.redF(), color.greenF(), color.blueF());
        _material->needsUpdate = true;
      }
      emit colorChanged();
    }
  }

  float opacity() const {return _opacity;}

  void setOpacity(float opacity) {
    if(_opacity != opacity) {
      _opacity = opacity;
      if(_material) _material->opacity = opacity;
      emit opacityChanged();
    }
  }

  float metalness() const {return _metalness;}

  void setMetalness(float metalness) {
    if(_metalness != metalness) {
      _metalness = metalness;
      if(_material) _material->metalness = metalness;
      emit metalnessChanged();
    }
  }

  float roughness() const {return _roughness;}

  void setRoughness(float roughness) {
    if(_roughness != roughness) {
      _roughness = roughness;
      if(_material) _material->roughness = roughness;
      emit roughnessChanged();
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

  Texture *lightMap() const {return _lightMap;}

  void setLightMap(Texture *lightMap) {
    if(_lightMap != lightMap) {
      _lightMap = lightMap;
      if(_material) {
        _material->lightMap = _lightMap ? _lightMap().getTexture() : nullptr;
        _material->needsUpdate = true;
      }
      emit lightMapChanged();
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

  three::MeshStandardMaterial::Ptr createMaterial()
  {
    _material = three::MeshStandardMaterial::make();
    _material->color = Color(_color().redF(), _color().greenF(), _color().blueF());
    _material->opacity = _opacity;
    _material->roughness = _roughness;
    _material->metalness = _metalness;
    _material->envMap = _envMap ? _envMap().getTexture() : nullptr;
    _material->lightMap = _lightMap ? _lightMap().getTexture() : nullptr;

    setBaseProperties(_material);

    return _material;
  }

  three::Material::Ptr getMaterial() override
  {
    return _material ? _material : createMaterial();
  }

signals:
  void colorChanged();
  void opacityChanged();
  void envMapChanged();
  void lightMapChanged();
  void metalnessChanged();
  void roughnessChanged();
  void ditheringChanged();
  void reflectivityChanged();
  void refractionRatioChanged();
};

}
}

#endif //THREE_PP_MESHSTANDARDMATERIAL_H
