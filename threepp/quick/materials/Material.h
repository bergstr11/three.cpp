//
// Created by byter on 12/14/17.
//

#ifndef THREEPPQ_MATERIAL_H
#define THREEPPQ_MATERIAL_H

#include <threepp/quick/ThreeQObjectRoot.h>
#include <threepp/quick/textures/Texture.h>
#include <threepp/quick/qutil/Resolver.h>
#include <threepp/quick/textures/ImageTexture.h>
#include <threepp/quick/qutil/TrackingProperty.h>

namespace three {
namespace quick {

template <typename QM>
struct Diffuse
{
  QM * const _q;

  TrackingProperty<QColor> _color {0xffffff};
  TrackingProperty<float> _opacity {1.0f};
  TrackingProperty<Texture *> _map {nullptr};

  explicit Diffuse(QM *qm) : _q(qm) {}

  QColor color() const {
    return _q->_material ? QColor::fromRgbF(_q->_material->color.r, _q->_material->color.g, _q->_material->color.b) : _color;
  }

  void setColor(const QColor &color)
  {
    if(_color != color) {
      _color = color;
      if(_q->_material) {
        _q->_material->color.set(_color().redF(), _color().greenF(), _color().blueF());
        _q->_material->needsUpdate = true;
      }
      emit _q->colorChanged();
    }
    else _color.set();
  }

  float opacity() const {return _q->_material ? _q->_material->opacity : _opacity;}

  void setOpacity(float opacity)
  {
    if(_opacity != opacity) {
      _opacity = opacity;
      if(_q->_material) {
        _q->_material->opacity = _opacity;
        _q->_material->needsUpdate = true;
      }
      emit _q->opacityChanged();
    }
    else _opacity.set();
  }

  Texture *map() const {return _map;}

  void setMap(Texture *map)
  {
    if(_map != map) {
      _map = map;
      if(_q->_material) {
        _q->_material->map = _map ? _map().getTexture() : nullptr;
        _q->_material->needsUpdate = true;
      }
      emit _q->mapChanged();
    }
    else _map.set();
  }

  template <typename M>
  void applyDiffuse(const std::shared_ptr<M> &m)
  {
    if(_color.isSet()) m->color.set(_color().redF(), _color().greenF(), _color().blueF());
    if(_opacity.isSet()) m->opacity = _opacity;
    if(_map.isSet()) m->map = _map().getTexture();
  }
};

template <typename QM>
struct LightMap
{
  QM * const _q;

  LightMap(QM *q) : _q(q) {}

  TrackingProperty<Texture *> _lightMap {nullptr};
  TrackingProperty<float> _lightMapIntensity {1.0};

  float lightMapIntensity() const {
    return _q->_material ? _q->_material->lightMapIntensity : _lightMapIntensity;
  }

  void setLightMapIntensity(float lightMapIntensity)
  {
    if(_lightMapIntensity != lightMapIntensity) {
      _lightMapIntensity = lightMapIntensity;
      if(_q->_material) _q->_material->lightMapIntensity = lightMapIntensity;
      emit _q->lightMapIntensityChanged();
    }
    else _lightMapIntensity.set();
  }

  Texture *lightMap()
  {
    if(!_lightMap && _q->_material && _q->_material->envMap) {
      const auto lm = _q->_material->lightMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _lightMap = new ImageTexture(it);
      }
    }
    return _lightMap;
  }

  void setLightMap(Texture *lightMap)
  {
    if(_lightMap != lightMap) {
      _lightMap = lightMap;
      if(_q->_material) {
        _q->_material->lightMap = _lightMap ? _lightMap().getTexture() : nullptr;
        _q->_material->needsUpdate = true;
      }
      emit _q->lightMapChanged();
    }
    else _lightMap.set();
  }

  template <typename M>
  void applyLightMap(const std::shared_ptr<M> &m)
  {
    if(_lightMapIntensity.isSet()) m->lightMapIntensity = _lightMapIntensity;
    if(_lightMap.isSet()) m->lightMap = _lightMap ? _lightMap().getTexture() : nullptr;
  }
};

template <typename QM>
struct Emissive
{
  QM * const _q;

  Emissive(QM *q) : _q(q) {}

  TrackingProperty<Texture *> _emissiveMap {nullptr};
  TrackingProperty<QColor> _emissive {QColor(0,0,0)};
  TrackingProperty<float> _emissiveIntensity {1.0};

  QColor emissive() const {
    return _q->_material ?
           QColor::fromRgbF(_q->_material->emissive.r, _q->_material->emissive.g, _q->_material->emissive.b) : _emissive;
  }

  void setEmissive(const QColor &emissive)
  {
    if(_emissive != emissive) {
      _emissive = emissive;
      if(_q->_material) {
        _q->_material->emissive.set(emissive.redF(), emissive.greenF(), emissive.blueF());
        _q->_material->needsUpdate = true;
      }
      emit _q->emissiveChanged();
    }
    else _emissive.set();
  }

  float emissiveIntensity() const {
    return _q->_material ? _q->_material->emissiveIntensity : _emissiveIntensity;
  }

  void setEmissiveIntensity(float emissiveIntensity)
  {
    if(_emissiveIntensity != emissiveIntensity) {
      _emissiveIntensity = emissiveIntensity;
      if(_q->_material) _q->_material->emissiveIntensity = emissiveIntensity;
      emit _q->emissiveIntensityChanged();
    }
    else _emissiveIntensity.set();
  }

  Texture *emissiveMap()
  {
    if(!_emissiveMap && _q->_material && _q->_material->envMap) {
      const auto lm = _q->_material->emissiveMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _emissiveMap = new ImageTexture(it);
      }
    }
    return _emissiveMap;
  }

  void setEmissiveMap(Texture *emissiveMap)
  {
    if(_emissiveMap != emissiveMap) {
      _emissiveMap = emissiveMap;
      if(_q->_material) {
        _q->_material->emissiveMap = _emissiveMap ? _emissiveMap().getTexture() : nullptr;
        _q->_material->needsUpdate = true;
      }
      emit _q->emissiveMapChanged();
    }
    else _emissiveMap.set();
  }


  template <typename M>
  void applyEmissive(const std::shared_ptr<M> &m)
  {
    if(_emissive.isSet()) m->emissive.set(_emissive().redF(), _emissive().greenF(), _emissive().blueF());
    if(_emissiveIntensity.isSet()) m->emissiveIntensity = _emissiveIntensity;
    if(_emissiveMap.isSet()) m->emissiveMap = _emissiveMap ? _emissiveMap().getTexture() : nullptr;
  }
};

struct AoMap
{
  TrackingProperty<Texture *> _aoMap {nullptr};
  TrackingProperty<float> _aoMapIntensity {1.0};
};

template <typename QM>
struct EnvMap
{
  QM * const _q;

  EnvMap(QM *q) : _q(q) {}

  TrackingProperty<Texture *> _envMap {nullptr};
  TrackingProperty<float> _reflectivity {1};
  TrackingProperty<float> _refractionRatio {0.98};
  TrackingProperty<float> _envMapIntensity {1.0f};

  Texture *envMap()
  {
    if(!_envMap && _q->_material && _q->_material->envMap) {
      const auto em = _q->_material->envMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(em)) {
        _envMap = new ImageTexture(it);
      }
    }
    return _envMap;
  }

  void setEnvMap(Texture *envMap) {
    if(_envMap != envMap) {
      _envMap = envMap;
      if(_q->_material) {
        _q->_material->envMap = _envMap ? _envMap().getTexture() : nullptr;
        _q->_material->needsUpdate = true;
      }
      emit _q->envMapChanged();
    }
    else _envMap.set();
  }

  float reflectivity() const
  {
    return _q->_material ? _q->_material->reflectivity : _reflectivity;
  }

  void setReflectivity(float reflectivity)
  {
    if(_reflectivity != reflectivity) {
      _reflectivity = reflectivity;
      if(_q->_material) {
        _q->_material->reflectivity = _reflectivity;
        _q->_material->needsUpdate = true;
      }
      emit _q->reflectivityChanged();
    }
    else _reflectivity.set();
  }

  float refractionRatio() const {return _q->_material ? _q->_material->refractionRatio : _refractionRatio;}

  void setRefractionRatio(float refractionRatio)
  {
    if(_refractionRatio != refractionRatio) {
      _refractionRatio = refractionRatio;
      if(_q->_material) {
        _q->_material->refractionRatio = _refractionRatio;
        _q->_material->needsUpdate = true;
      }
      emit _q->refractionRatioChanged();
    }
    else _refractionRatio.set();
  }


  template <typename M>
  void applyEnvMap(const std::shared_ptr<M> &m)
  {
    if(_envMap.isSet()) m->envMap = _envMap().getTexture();
    if(_reflectivity.isSet()) m->reflectivity = _reflectivity;
    if(_refractionRatio.isSet()) m->refractionRatio = _refractionRatio;
    if(_envMapIntensity.isSet()) m->envMapIntensity = _envMapIntensity;
  }
};

struct AlphaMap
{
  TrackingProperty<Texture *> _alphaMap {nullptr};
};

struct SpecularMap
{
  TrackingProperty<float> shininess {30};
  TrackingProperty<QColor> _specular {0x111111};
  TrackingProperty<Texture *> _specularMap {nullptr};
};

struct DisplacementMap
{
  TrackingProperty<Texture *> _displacementMap {nullptr};
  TrackingProperty<float> _displacementScale {1};
  TrackingProperty<float> _displacementBias {0};
};

struct BumpMap
{
  TrackingProperty<Texture *> _bumpMap {nullptr};
  TrackingProperty<float> _bumpScale {1};
};

template <typename QM>
struct NormalMap
{
  QM * const _q;

  NormalMap(QM *q) : _q(q) {}

  TrackingProperty<Texture *> _normalMap {nullptr};

  Texture *normalMap() {
    if(!_normalMap && _q->_material && _q->_material->normalMap) {
      const auto lm = _q->_material->normalMap;
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(lm)) {
        _normalMap = new ImageTexture(it);
      }
    }
    return _normalMap;
  }

  void setNormalMap(Texture *normalMap) {
    if(_normalMap != normalMap) {
      _normalMap = normalMap;
      if(_q->_material) {
        _q->_material->normalMap = _normalMap ? _normalMap().getTexture() : nullptr;
        _q->_material->needsUpdate = true;
      }
      emit _q->normalMapChanged();
    }
    else _normalMap.set();
  }

  template <typename M>
  void applyNormalMap(const std::shared_ptr<M> &m)
  {
    if(_normalMap.isSet()) m->normalMap = _normalMap().getTexture();
  }
};

struct RoughnessMap
{
  TrackingProperty<float> _roughness {0.5f};
  TrackingProperty<Texture *> _roughnessMap {nullptr};
};

struct MetalnessMap
{
  TrackingProperty<float> _metalness {0.5f};
  TrackingProperty<Texture *> _metalnessMap {nullptr};
};

class Material : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)
  Q_PROPERTY(bool flatShading READ flatShading WRITE setFlatShading NOTIFY flatShadingChanged)
  Q_PROPERTY(bool needsUpdate READ needsUpdate WRITE setNeedsUpdate NOTIFY needsUpdateChanged)
  Q_PROPERTY(bool dithering READ dithering WRITE setDithering NOTIFY ditheringChanged)
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
  Q_PROPERTY(QByteArray name READ name WRITE setName NOTIFY nameChanged)

protected:
  TrackingProperty<bool> _wireframe {false};
  TrackingProperty<bool> _flatShading {false};
  TrackingProperty<QByteArray> _name {QByteArray()};
  TrackingProperty<bool> _visible {true};
  TrackingProperty<bool> _dithering {false};

  Material(const material::Typer &typer, QObject *parent = nullptr) : ThreeQObjectRoot(parent), typer(typer) {}

  virtual three::Material::Ptr material() const = 0;

  bool needsUpdate() const {return material() ? material()->needsUpdate : true;}

  void setNeedsUpdate(bool value) {
    three::Material::Ptr mat = material();
    if(mat && mat->needsUpdate != value) {
      mat->needsUpdate = value;
      emit needsUpdateChanged();
    }
  }

  void setBaseProperties(three::Material::Ptr material);

public:
  const material::Typer typer;

  virtual void applyColor(const QColor &color) {}

  virtual void setAndConfigureObject(three::Material::Ptr material)
  {
    if(_wireframe.isSet()) material->wireframe = _wireframe;
    if(_flatShading.isSet()) material->flatShading = _flatShading;
    if(_name.isSet()) material->name = _name().toStdString();
    if(_visible.isSet()) material->visible = _visible;
    if(_dithering.isSet()) material->dithering = _dithering;
  }

  bool visible() const {return _visible;}

  void setVisible(bool visible) {
    if(_visible != visible) {
      _visible = visible;
      if(material()) {
        material()->visible = _visible;
      }
      emit visibleChanged();
    }
  }

  bool wireframe() const {return _wireframe;}

  void setWireframe(bool wireframe) {
    if(_wireframe != wireframe) {
      _wireframe = wireframe;
      emit wireframeChanged();
    }
  }

  bool flatShading() const {return _flatShading;}

  void setFlatShading(bool flatShading) {
    if(_flatShading != flatShading) {
      _flatShading = flatShading;
      emit flatShadingChanged();
    }
  }

  bool dithering() const {return material() ? material()->dithering : _dithering;}

  void setDithering(bool dithering) {
    if(_dithering != dithering) {
      _dithering = dithering;
      if(material()) {
        material()->dithering = _dithering;
        material()->needsUpdate = true;
      }
      emit ditheringChanged();
    }
  }


  const QByteArray &name() const {return _name();}

  void setName(const QByteArray &name) {
    if(_name != name) {
      _name = name;
      emit nameChanged();
    }
  }

  virtual three::Material::Ptr getMaterial() = 0;

  Q_INVOKABLE QString getInfo();

signals:
  void wireframeChanged();
  void flatShadingChanged();
  void needsUpdateChanged();
  void mapChanged();
  void ditheringChanged();
  void visibleChanged();
  void nameChanged();
};

}
}

#endif //THREEPPQ_MATERIAL_H
