//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MATERIAL_H
#define THREEPP_MATERIAL_H

#include <QObject>

namespace three {
namespace quick {

class Material : public QObject
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

  virtual three::Material::Ptr create() = 0;

signals:
  void wireframeChanged();
};

}
}

#endif //THREEPP_MATERIAL_H
