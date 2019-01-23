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

class MeshStandardMaterial : public Material,
   public Diffuse<MeshStandardMaterial>,
   public Emissive<MeshStandardMaterial>,
   public EnvMap<MeshStandardMaterial>
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(three::quick::Texture *map READ map WRITE setMap NOTIFY mapChanged)
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

  TrackingProperty<float> _roughness {0.5f};
  TrackingProperty<float> _metalness {0.5f};
  TrackingProperty<Texture *> _lightMap {nullptr};
  TrackingProperty<Texture *> _metalnessMap {nullptr};
  TrackingProperty<Texture *> _normalMap {nullptr};
  TrackingProperty<Texture *> _roughnessMap {nullptr};

  TrackingProperty<bool> _dithering {false};
  TrackingProperty<float> _refractionRatio {0.98f};

protected:
  three::Material::Ptr material() const override {return _material;}

  void setProperties(three::MeshStandardMaterial::Ptr material)
  {
    setBaseProperties(material);
    applyDiffuse(material);
    applyEmissive(material);
    applyEnvMap(material);

    material->roughness = _roughness;
    material->metalness = _metalness;
    material->dithering = _dithering;
    material->refractionRatio = _refractionRatio;
    material->lightMap = _lightMap ? _lightMap().getTexture() : nullptr;
    material->normalMap = _normalMap ? _normalMap().getTexture() : nullptr;
    material->metalnessMap = _metalnessMap ? _metalnessMap().getTexture() : nullptr;
    material->roughnessMap = _roughnessMap ? _roughnessMap().getTexture() : nullptr;
  }

  MeshStandardMaterial(three::MeshStandardMaterial::Ptr mat, const material::Typer &typer, QObject *parent = nullptr)
     : Material(typer, parent), Diffuse(this), Emissive(this), EnvMap(this), _material(mat)
  {}

  MeshStandardMaterial(const material::Typer &typer, QObject *parent=nullptr)
     : Material(typer, parent), Diffuse(this), Emissive(this), EnvMap(this) {}

public:
  three::MeshStandardMaterial::Ptr _material;

  MeshStandardMaterial(three::MeshStandardMaterial::Ptr mat, QObject *parent = nullptr)
     : Material(material::Typer(this), parent), Diffuse(this), Emissive(this), EnvMap(this), _material(mat)
  {}

  MeshStandardMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent), Diffuse(this), Emissive(this), EnvMap(this) {}

  void applyColor(const QColor &color)
  {
    setColor(color);
  }

  void setAndConfigureObject(three::Material::Ptr material) override
  {
    _material = std::dynamic_pointer_cast<three::MeshStandardMaterial>(material);
    if(!_material) {
      qCritical() << "MaterialHandler: received incompatible material";
    }
    Material::setAndConfigureObject(material);
    applyDiffuse(_material);
    applyEnvMap(_material);
    applyEmissive(_material);

    if(_roughness.isSet()) _material->roughness = _roughness;
    if(_metalness.isSet()) _material->metalness = _metalness;
    if(_lightMap.isSet()) _material->lightMap = _lightMap().getTexture();
    if(_normalMap.isSet()) _material->normalMap = _normalMap().getTexture();
    if(_metalnessMap.isSet()) _material->metalnessMap = _metalnessMap().getTexture();
    if(_roughnessMap.isSet()) _material->roughnessMap = _roughnessMap().getTexture();
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

  Texture *lightMap() {
    if(!_lightMap && _material && _material->lightMap) {
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
    if(!_normalMap && _material && _material->normalMap) {
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

  Texture *metalnessMap() {
    if(!_metalnessMap && _material && _material->metalnessMap) {
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
    if(!_roughnessMap && _material && _material->roughnessMap) {
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
  void mapChanged();
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
