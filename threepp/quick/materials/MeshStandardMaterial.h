//
// Created by byter on 5/23/18.
//

#ifndef THREE_PP_MESHSTANDARDMATERIAL_H
#define THREE_PP_MESHSTANDARDMATERIAL_H

#include <QColor>
#include <threepp/material/MeshStandardMaterial.h>
#include <threepp/quick/textures/ImageTexture.h>

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
  Q_PROPERTY(QColor emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
  Q_PROPERTY(float emissiveIntensity READ emissiveIntensity WRITE setEmissiveIntensity NOTIFY emissiveIntensityChanged)
  Q_PROPERTY(Texture *emissiveMap READ emissiveMap WRITE setEmissiveMap NOTIFY emissiveMapChanged)
  Q_PROPERTY(Texture *envMap READ envMap WRITE setEnvMap NOTIFY envMapChanged)
  Q_PROPERTY(Texture *lightMap READ lightMap WRITE setLightMap NOTIFY lightMapChanged)
  Q_PROPERTY(Texture *metalnessMap READ metalnessMap WRITE setMetalnessMap NOTIFY metalnessMapChanged)
  Q_PROPERTY(Texture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
  Q_PROPERTY(Texture *roughnessMap READ roughnessMap WRITE setRoughnessMap NOTIFY roughnessMapChanged)
  Q_PROPERTY(float refractionRatio READ refractionRatio WRITE setRefractionRatio NOTIFY refractionRatioChanged)

  TrackingProperty<QColor> _color {QColor(255,255,255)};
  TrackingProperty<float> _opacity {1.0f};
  TrackingProperty<float> _roughness {0.5f};
  TrackingProperty<float> _metalness {0.5f};
  TrackingProperty<QColor> _emissive {QColor(0,0,0)};
  TrackingProperty<float> _emissiveIntensity {1.0f};

  TrackingProperty<Texture *> _envMap {nullptr};
  TrackingProperty<Texture *> _lightMap {nullptr};
  TrackingProperty<Texture *> _metalnessMap {nullptr};
  TrackingProperty<Texture *> _normalMap {nullptr};
  TrackingProperty<Texture *> _roughnessMap {nullptr};
  TrackingProperty<Texture *> _emissiveMap {nullptr};

  TrackingProperty<float> _reflectivity {1};
  TrackingProperty<bool> _dithering {false};
  TrackingProperty<float> _refractionRatio {0.98f};

  three::MeshStandardMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

  void setProperties(three::MeshStandardMaterial::Ptr material)
  {
    material->color = Color(_color().redF(), _color().greenF(), _color().blueF());
    material->opacity = _opacity;
    material->roughness = _roughness;
    material->metalness = _metalness;
    material->dithering = _dithering;
    material->reflectivity = _reflectivity;
    material->refractionRatio = _refractionRatio;
    material->envMap = _envMap ? _envMap().getTexture() : nullptr;
    material->lightMap = _lightMap ? _lightMap().getTexture() : nullptr;
    material->normalMap = _normalMap ? _normalMap().getTexture() : nullptr;
    material->metalnessMap = _metalnessMap ? _metalnessMap().getTexture() : nullptr;
    material->roughnessMap = _roughnessMap ? _roughnessMap().getTexture() : nullptr;
    material->emissive.set(_emissive().redF(), _emissive().greenF(), _emissive().blueF());
    material->emissiveIntensity = _emissiveIntensity;
    material->emissiveMap = _emissiveMap ? _emissiveMap().getTexture() : nullptr;

    setBaseProperties(material);
  }

  MeshStandardMaterial(three::MeshStandardMaterial::Ptr mat, const material::Typer &typer, QObject *parent = nullptr)
     : Material(typer, parent), _material(mat)
  {}

  MeshStandardMaterial(const material::Typer &typer, QObject *parent=nullptr)
     : Material(typer, parent) {}

public:
  MeshStandardMaterial(three::MeshStandardMaterial::Ptr mat, QObject *parent = nullptr)
     : Material(material::Typer(this), parent), _material(mat)
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
    if(_normalMap.isSet()) _material->normalMap = _normalMap().getTexture();
    if(_metalnessMap.isSet()) _material->metalnessMap = _metalnessMap().getTexture();
    if(_roughnessMap.isSet()) _material->roughnessMap = _roughnessMap().getTexture();
    if(_emissive.isSet()) _material->emissive.set(_emissive().redF(), _emissive().greenF(), _emissive().blueF());
    if(_emissiveIntensity.isSet()) _material->emissiveIntensity = _emissiveIntensity;
    if(_emissiveMap.isSet()) _material->emissiveMap = _emissiveMap ? _emissiveMap().getTexture() : nullptr;
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

  QColor emissive() const
  {
    return _material ?
           QColor::fromRgbF(_material->emissive.r, _material->emissive.g, _material->emissive.b) : _emissive;
  }

  void setEmissive(const QColor &emissive) {
    if(_emissive != emissive) {
      _emissive = emissive;
      if(_material) {
        _material->emissive.set(emissive.redF(), emissive.greenF(), emissive.blueF());
        _material->needsUpdate = true;
      }
      emit emissiveChanged();
    }
  }

  float emissiveIntensity() const {return _material ? _material->emissiveIntensity : _emissiveIntensity;}

  void setEmissiveIntensity(float emissiveIntensity) {
    if(_emissiveIntensity != emissiveIntensity) {
      _emissiveIntensity = emissiveIntensity;
      if(_material) _material->emissiveIntensity = emissiveIntensity;
      emit emissiveIntensityChanged();
    }
  }

  float opacity() const {return _material ? _material->opacity : _opacity;}

  void setOpacity(float opacity) {
    if(_opacity != opacity) {
      _opacity = opacity;
      if(_material) _material->opacity = opacity;
      emit opacityChanged();
    }
  }

  float metalness() const {return _material ? _material->metalness : _metalness;}

  void setMetalness(float metalness) {
    if(_metalness != metalness) {
      _metalness = metalness;
      if(_material) _material->metalness = metalness;
      emit metalnessChanged();
    }
  }

  float roughness() const {return _material ? _material->roughness : _roughness;}

  void setRoughness(float roughness) {
    if(_roughness != roughness) {
      _roughness = roughness;
      if(_material) _material->roughness = roughness;
      emit roughnessChanged();
    }
  }

  Texture *envMap() {
    if(!_envMap && _material && _material->envMap) {
      const auto em = _material->envMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(em)) {
        _envMap = new ImageTexture(it);
      }
    }
    return _envMap;
  }

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

  Texture *lightMap() {
    if(!_lightMap && _material && _material->envMap) {
      const auto lm = _material->lightMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _lightMap = new ImageTexture(it);
      }
    }
    return _lightMap;
  }

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

  Texture *normalMap() {
    if(!_normalMap && _material && _material->envMap) {
      const auto lm = _material->normalMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _normalMap = new ImageTexture(it);
      }
    }
    return _normalMap;
  }

  void setNormalMap(Texture *normalMap) {
    if(_normalMap != normalMap) {
      _normalMap = normalMap;
      if(_material) {
        _material->normalMap = _normalMap ? _normalMap().getTexture() : nullptr;
        _material->needsUpdate = true;
      }
      emit normalMapChanged();
    }
  }

  Texture *emissiveMap() {
    if(!_emissiveMap && _material && _material->envMap) {
      const auto lm = _material->emissiveMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _emissiveMap = new ImageTexture(it);
      }
    }
    return _emissiveMap;
  }

  void setEmissiveMap(Texture *emissiveMap) {
    if(_emissiveMap != emissiveMap) {
      _emissiveMap = emissiveMap;
      if(_material) {
        _material->emissiveMap = _emissiveMap ? _emissiveMap().getTexture() : nullptr;
        _material->needsUpdate = true;
      }
      emit emissiveMapChanged();
    }
  }

  Texture *metalnessMap() {
    if(!_metalnessMap && _material && _material->envMap) {
      const auto lm = _material->metalnessMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _metalnessMap = new ImageTexture(it);
      }
    }
    return _metalnessMap;
  }

  void setMetalnessMap(Texture *metalnessMap) {
    if(_metalnessMap != metalnessMap) {
      _metalnessMap = metalnessMap;
      if(_material) {
        _material->metalnessMap = _metalnessMap ? _metalnessMap().getTexture() : nullptr;
        _material->needsUpdate = true;
      }
      emit metalnessMapChanged();
    }
  }

  Texture *roughnessMap() {
    if(!_roughnessMap && _material && _material->envMap) {
      const auto lm = _material->roughnessMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _roughnessMap = new ImageTexture(it);
      }
    }
    return _roughnessMap;
  }

  void setRoughnessMap(Texture *roughnessMap) {
    if(_roughnessMap != roughnessMap) {
      _roughnessMap = roughnessMap;
      if(_material) {
        _material->roughnessMap = _roughnessMap ? _roughnessMap().getTexture() : nullptr;
        _material->needsUpdate = true;
      }
      emit roughnessMapChanged();
    }
  }

  bool dithering() const {return _material ? _material->dithering : _dithering;}

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

  float reflectivity() const {return _material ? _material->reflectivity : _reflectivity;}

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

  three::MeshStandardMaterial::Ptr createMeshStandardMaterial()
  {
    auto material = three::MeshStandardMaterial::make();
    setProperties(material);
    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMeshStandardMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshStandardMaterial::Ptr(_material->cloned()) : createMeshStandardMaterial();
    return new MeshStandardMaterial(material);
  }

signals:
  void emissiveChanged();
  void emissiveIntensityChanged();
  void emissiveMapChanged();
  void colorChanged();
  void opacityChanged();
  void envMapChanged();
  void lightMapChanged();
  void metalnessChanged();
  void roughnessChanged();
  void ditheringChanged();
  void reflectivityChanged();
  void refractionRatioChanged();
  void metalnessMapChanged();
  void normalMapChanged();
  void roughnessMapChanged();
};

}
}

#endif //THREE_PP_MESHSTANDARDMATERIAL_H
