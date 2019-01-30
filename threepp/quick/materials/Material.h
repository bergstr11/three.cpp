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

#define COLOR_PROPERTY(M_name, M_Name, M_init) \
TrackingProperty<QColor> _##M_name {M_init}; \
QColor M_name() const {return _q->_material ? QColor::fromRgbF(_q->_material->M_name.r, _q->_material->M_name.g, _q->_material->M_name.b) : _##M_name;} \
void set##M_Name(const QColor &M_name) { \
  if(_##M_name != M_name) { \
    _##M_name = M_name; \
    if(_q->_material) { \
      _q->_material->M_name.set(_##M_name().redF(), _##M_name().greenF(), _##M_name().blueF()); \
      _q->_material->needsUpdate = true; \
    } \
    emit _q->M_name##Changed(); \
  } \
  else _##M_name.set(); \
}
#define APPLY_COLOR(M_name) if(_##M_name.isSet()) m->M_name.set(_##M_name().redF(), _##M_name().greenF(), _##M_name().blueF());

#define FLOAT_PROPERTY(M_name, M_Name, M_init) \
TrackingProperty<float> _##M_name {M_init}; \
float M_name() const {return _q->_material ? _q->_material->M_name : _##M_name;} \
void set##M_Name(float M_name) { \
  if(_##M_name != M_name) { \
    _##M_name = M_name; \
    if(_q->_material) { \
      _q->_material->M_name = _##M_name; \
      _q->_material->needsUpdate = true; \
    } \
    emit _q->M_name##Changed(); \
  } \
  else _##M_name.set(); \
}
#define APPLY_FLOAT(M_name) if(_##M_name.isSet()) m->M_name = _##M_name;

#define MAP_PROPERTY(M_name, M_Name) \
TrackingProperty<Texture *> _##M_name {nullptr}; \
Texture *M_name() \
  { \
    if(_q->_material && _q->_material->M_name && (!_##M_name || _##M_name().getTexture() != _q->_material->M_name)) { \
      const auto m = _q->_material->M_name; \
      if(three::ImageTexture::Ptr it = std::dynamic_pointer_cast<three::ImageTexture>(m)) { \
        _##M_name = new ImageTexture(it); \
      } \
    } \
    return _##M_name; \
  } \
void set##M_Name(Texture *M_name) { \
  if(_##M_name != M_name) { \
    _##M_name = M_name; \
    if(_q->_material) { \
      _q->_material->M_name = _##M_name ? _##M_name().getTexture() : nullptr; \
      _q->_material->needsUpdate = true; \
    } \
    emit _q->M_name##Changed(); \
  } \
  else _##M_name.set(); \
}
#define APPLY_MAP(M_name) if(_##M_name.isSet()) m->M_name = _##M_name ? _##M_name().getTexture() : nullptr;

template <typename QM>
struct Diffuse
{
  QM * const _q;

  explicit Diffuse(QM *qm) : _q(qm) {}

  COLOR_PROPERTY(color, Color, 0xffffff)
  FLOAT_PROPERTY(opacity, Opacity, 1.0f)
  MAP_PROPERTY(map, Map)

  template <typename M>
  void applyDiffuse(const std::shared_ptr<M> &m)
  {
    APPLY_COLOR(color)
    APPLY_FLOAT(opacity)
    APPLY_MAP(map)
  }
};

template <typename QM>
struct LightMap
{
  QM * const _q;

  LightMap(QM *q) : _q(q) {}

  FLOAT_PROPERTY(lightMapIntensity, LightMapIntensity, 1.0f)
  MAP_PROPERTY(lightMap, LightMap)

  template <typename M>
  void applyLightMap(const std::shared_ptr<M> &m)
  {
    APPLY_FLOAT(lightMapIntensity)
    APPLY_MAP(lightMap)
  }
};

template <typename QM>
struct Emissive
{
  QM * const _q;

  Emissive(QM *q) : _q(q) {}

  COLOR_PROPERTY(emissive, Emissive, 0x0d)
  FLOAT_PROPERTY(emissiveIntensity, EmissiveIntensity, 1.0f)
  MAP_PROPERTY(emissiveMap, EmissiveMap)


  template <typename M>
  void applyEmissive(const std::shared_ptr<M> &m)
  {
    APPLY_COLOR(emissive)
    APPLY_FLOAT(emissiveIntensity)
    APPLY_MAP(emissiveMap)
  }
};

template <typename QM>
struct EnvMap
{
  QM * const _q;

  EnvMap(QM *q) : _q(q) {}

  MAP_PROPERTY(envMap, EnvMap)
  FLOAT_PROPERTY(reflectivity, Reflectivity, 1)
  FLOAT_PROPERTY(refractionRatio, RefractionRatio, 0.98f)
  FLOAT_PROPERTY(envMapIntensity, EnvMapIntensity, 1.0f)

  template <typename M>
  void applyEnvMap(const std::shared_ptr<M> &m)
  {
    APPLY_MAP(envMap)
    APPLY_FLOAT(reflectivity)
    APPLY_FLOAT(refractionRatio)
    APPLY_FLOAT(envMapIntensity)
  }
};

template <typename QM>
struct NormalMap
{
  QM * const _q;
  NormalMap(QM *q) : _q(q) {}

  MAP_PROPERTY(normalMap, NormalMap)

  template <typename M>
  void applyNormalMap(const std::shared_ptr<M> &m)
  {
    APPLY_MAP(normalMap)
  }
};

template <typename QM>
struct AoMap
{
  QM * const _q;
  AoMap(QM *q) : _q(q) {}

  MAP_PROPERTY(aoMap, AoMap)
  FLOAT_PROPERTY(aoMapIntensity, AoMapIntensity, 1.0f)

  template <typename M>
  void applyAoMap(const std::shared_ptr<M> &m)
  {
    APPLY_MAP(aoMap)
    APPLY_FLOAT(aoMapIntensity)
  }
};

template <typename QM>
struct AlphaMap
{
  QM * const _q;
  AlphaMap(QM *q) : _q(q) {}

  MAP_PROPERTY(alphaMap, AlphaMap)

  template <typename M>
  void applyAlphaMap(const std::shared_ptr<M> &m)
  {
    APPLY_MAP(alphaMap)
  }
};

template <typename QM>
struct SpecularMap
{
  QM * const _q;
  SpecularMap(QM *q) : _q(q) {}

  MAP_PROPERTY(specularMap, SpecularMap)

  template <typename M>
  void applySpecularMap(const std::shared_ptr<M> &m)
  {
    APPLY_MAP(specularMap)
  }
};

template <typename QM>
struct Specular
{
  QM * const _q;
  Specular(QM *q) : _q(q) {}

  MAP_PROPERTY(specularMap, SpecularMap)
  FLOAT_PROPERTY(shininess, Shininess, 30.0f)
  COLOR_PROPERTY(specular, Specular, 0x111111)

  template <typename M>
  void applySpecular(const std::shared_ptr<M> &m)
  {
    APPLY_MAP(specularMap)
    APPLY_FLOAT(shininess)
    APPLY_COLOR(specular)
  }
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
