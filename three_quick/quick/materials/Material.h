//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MATERIAL_H
#define THREEPP_MATERIAL_H

#include <quick/ThreeQObjectRoot.h>
#include <quick/objects/MeshCreator.h>
#include "../textures/Texture.h"

namespace three {
namespace quick {

class Material : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)
  Q_PROPERTY(bool flatShading READ flatShading WRITE setFlatShading NOTIFY flatShadingChanged)
  Q_PROPERTY(bool needsUpdate READ needsUpdate WRITE setNeedsUpdate NOTIFY needsUpdateChanged)
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
  Q_PROPERTY(Texture *map READ map WRITE setMap NOTIFY mapChanged)

protected:
  bool _wireframe = false;
  bool _flatShading = false;
  bool _visible = true;
  Texture *_map = nullptr;

  Material(QObject *parent = nullptr) : ThreeQObjectRoot(parent) {}

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

  Texture *map() const {return _map;}

  void setMap(Texture *map) {
    if(_map != map) {
      _map = map;
      if(material()) {
        material()->map = _map->getTexture();
        material()->needsUpdate = true;
      }
      emit mapChanged();
    }
  }

  virtual void identify(MeshCreator &creator) = 0;

signals:
  void wireframeChanged();
  void flatShadingChanged();
  void needsUpdateChanged();
  void mapChanged();
  void visibleChanged();
};

}
}

#endif //THREEPP_MATERIAL_H
