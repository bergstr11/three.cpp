//
// Created by byter on 12/29/18.
//

#ifndef THREE_PP_MESHPHYSICALMATERIAL_H
#define THREE_PP_MESHPHYSICALMATERIAL_H

#include <threepp/quick/materials/MeshStandardMaterial.h>
#include <threepp/material/MeshPhysicalMaterial.h>

namespace three {
namespace quick {

class MeshPhysicalMaterial : public MeshStandardMaterial
{
Q_OBJECT
  Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
  Q_PROPERTY(float clearCoat READ clearCoat WRITE setClearCoat NOTIFY clearCoatChanged)
  Q_PROPERTY(float clearCoatRoughness READ clearCoatRoughness WRITE setClearCoatRoughness NOTIFY clearCoatRoughnessChanged)

  TrackingProperty<float> _reflectivity {0.5}; // maps to F0 = 0.04
  TrackingProperty<float> _clearCoat {0.0};
  TrackingProperty<float> _clearCoatRoughness {0.0};

  three::MeshPhysicalMaterial::Ptr _material;

protected:
  void setProperties(three::MeshPhysicalMaterial::Ptr material)
  {
    MeshStandardMaterial::setProperties(material);

    material->reflectivity = _reflectivity();
    material->clearCoat = _clearCoat();
    material->clearCoatRoughness = _clearCoatRoughness();
  }

public:
  MeshPhysicalMaterial(three::MeshPhysicalMaterial::Ptr mat, QObject *parent = nullptr)
  : MeshStandardMaterial(mat, material::Typer(this), parent), _material(mat) {}

  MeshPhysicalMaterial(QObject *parent=nullptr)
  : MeshStandardMaterial(material::Typer(this), parent) {}

  void setAndConfigureObject(three::Material::Ptr material) override
  {
    MeshStandardMaterial::setAndConfigureObject(material);

    _material = std::dynamic_pointer_cast<three::MeshPhysicalMaterial>(material);
    if(!_material) {
      qCritical() << "MaterialHandler: received incompatible material. Double handled?";
    }
    if(_reflectivity.isSet()) _material->reflectivity = _reflectivity;
    if(_clearCoat.isSet()) _material->clearCoat = _clearCoat;
    if(_clearCoatRoughness.isSet()) _material->clearCoatRoughness = _clearCoatRoughness;
  }

  float reflectivity() const {return _material ? _material->reflectivity : _reflectivity;}

  void setReflectivity(float reflectivity) {
    if(_reflectivity != reflectivity) {
      _reflectivity = reflectivity;
      if(_material) _material->reflectivity = reflectivity;
      emit reflectivityChanged();
    }
  }

  float clearCoat() const {return _material ? _material->clearCoat : _clearCoat;}

  void setClearCoat(float clearCoat) {
    if(_clearCoat != clearCoat) {
      _clearCoat = clearCoat;
      if(_material) _material->clearCoat = clearCoat;
      emit clearCoatChanged();
    }
  }

  float clearCoatRoughness() const {return _material ? _material->clearCoatRoughness : _clearCoatRoughness;}

  void setClearCoatRoughness(float clearCoatRoughness) {
    if(_clearCoatRoughness != clearCoatRoughness) {
      _clearCoatRoughness = clearCoatRoughness;
      if(_material) _material->clearCoatRoughness = clearCoatRoughness;
      emit clearCoatRoughnessChanged();
    }
  }

  three::MeshPhysicalMaterial::Ptr createMeshPhysicalMaterial()
  {
    auto material = three::MeshPhysicalMaterial::make();
    setProperties(material);
    return material;
  }

  three::Material::Ptr getMaterial() override
  {
    if(!_material) _material = createMeshPhysicalMaterial();
    return _material;
  }

  Q_INVOKABLE QObject *cloned() {
    auto material = _material ? three::MeshPhysicalMaterial::Ptr(_material->cloned()) : createMeshPhysicalMaterial();
    return new MeshPhysicalMaterial(material);
  }

signals:
  void reflectivityChanged();
  void clearCoatChanged();
  void clearCoatRoughnessChanged();
};

}
}

#endif //THREE_PP_MESHPHYSICALMATERIAL_H
