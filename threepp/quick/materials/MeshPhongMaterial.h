//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_MESHPHONGMATERIAL_H
#define THREEPPQ_MESHPHONGMATERIAL_H

#include <QColor>
#include <threepp/material/MeshPhongMaterial.h>
#include <threepp/quick/textures/ImageTexture.h>

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
  Q_PROPERTY(QColor emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
  Q_PROPERTY(float emissiveIntensity READ emissiveIntensity WRITE setEmissiveIntensity NOTIFY emissiveIntensityChanged)
  Q_PROPERTY(Texture *emissiveMap READ emissiveMap WRITE setEmissiveMap NOTIFY emissiveMapChanged)

  TrackingProperty<QColor> _color {0x111111};
  TrackingProperty<float> _opacity {1.0f};
  TrackingProperty<QColor> _specular {0x111111};
  TrackingProperty<float> _shininess {30};
  TrackingProperty<float> _reflectivity {1};
  TrackingProperty<bool> _dithering {false};
  TrackingProperty<float> _refractionRatio {0.98f};
  TrackingProperty<QColor> _emissive {QColor(0,0,0)};
  TrackingProperty<float> _emissiveIntensity {1.0f};

  TrackingProperty<Texture *>_normalMap {nullptr};
  TrackingProperty<Texture *> _envMap {nullptr};
  TrackingProperty<Texture *> _emissiveMap {nullptr};

  three::MeshPhongMaterial::Ptr _material;

protected:
  three::Material::Ptr material() const override {return _material;}

public:
  MeshPhongMaterial(three::MeshPhongMaterial::Ptr mat, QObject *parent = nullptr)
     : Material(material::Typer(this), parent), _material(mat)
  {}

  MeshPhongMaterial(QObject *parent=nullptr)
     : Material(material::Typer(this), parent) {}

  QColor color() const {
    return _material ? QColor::fromRgbF(_material->color.r, _material->color.g, _material->color.b) : _color;
  }

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

  QColor specular() const {
    return _material ? QColor::fromRgbF(_material->specular.r, _material->specular.g, _material->specular.b) : _specular;
  }

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

  float opacity() const {return _material ? _material->opacity : _opacity;}

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

  float shininess() const {return _material ? _material->shininess : _shininess;}

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

  float refractionRatio() const {return _material ? _material->refractionRatio : _refractionRatio;}

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

  Texture *normalMap() {
    if(!_normalMap && _material && _material->normalMap) {
      const auto nm = _material->normalMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(nm)) {
        _normalMap = new ImageTexture(it);
      }
    }
    return _normalMap;
  }

  void setNormalMap(Texture *normalMap) {
    if(_normalMap != normalMap) {
      _normalMap = normalMap;
      if(_material) {
        _material->normalMap = _normalMap().getTexture();
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
    auto material = three::MeshPhongMaterial::make(Color(_color().redF(), _color().greenF(), _color().blueF()), _dithering);
    material->opacity = _opacity;
    material->specular.set(_specular().redF(), _specular().greenF(), _specular().blueF());
    material->refractionRatio = _refractionRatio;
    material->shininess = _shininess;
    material->reflectivity = _reflectivity;
    material->dithering = _dithering;
    material->emissive.set(_emissive().redF(), _emissive().greenF(), _emissive().blueF());
    material->emissiveIntensity = _emissiveIntensity;

    if(_normalMap) material->normalMap = _normalMap().getTexture();
    if(_envMap) material->envMap = _envMap().getTexture();
    if(_emissiveMap) material->emissiveMap = _emissiveMap().getTexture();

    setBaseProperties(material);

    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshPhongMaterial::Ptr(_material->cloned()) : createMaterial();
    return new MeshPhongMaterial(material);
  }

signals:
  void colorChanged();
  void specularChanged();
  void emissiveChanged();
  void emissiveIntensityChanged();
  void emissiveMapChanged();
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
