//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MATERIAL_H
#define THREEPP_MATERIAL_H

#include <quick/ThreeQObjectRoot.h>
#include <quick/objects/MeshCreator.h>

namespace three {
namespace quick {

class Material : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)
  Q_PROPERTY(bool needsUpdate READ needsUpdate WRITE setNeedsUpdate NOTIFY needsUpdateChanged)

protected:
  bool _wireframe = false;

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

public:
  bool wireframe() const {return _wireframe;}

  void setWireframe(bool wireframe) {
    if(_wireframe != wireframe) {
      _wireframe = wireframe;
      emit wireframeChanged();
    }
  }

  virtual void identify(MeshCreator &creator) = 0;

signals:
  void wireframeChanged();
  void needsUpdateChanged();
};

}
}

#endif //THREEPP_MATERIAL_H
