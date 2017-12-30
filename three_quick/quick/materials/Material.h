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

protected:
  bool _wireframe = false;

public:
  bool wireframe() const {return _wireframe;}

  void setWireframe(bool wireframe) {
    if(_wireframe != wireframe) {
      _wireframe = wireframe;
      emit wireframeChanged();
    }
  }

  virtual void identify(MeshCreator *creator) = 0;

signals:
  void wireframeChanged();
};

}
}

#endif //THREEPP_MATERIAL_H
