//
// Created by byter on 12/29/17.
//

#ifndef THREEPP_SHADERMATERIAL_H
#define THREEPP_SHADERMATERIAL_H

#include <QVariantMap>
#include <material/ShaderMaterial.h>
#include <renderers/gl/shader/ShaderLib.h>
#include "../Three.h"
#include "Material.h"

namespace three {
namespace quick {

class ShaderMaterial : public Material
{
Q_OBJECT
  Q_PROPERTY(QString shaderID READ shaderID WRITE setShaderID NOTIFY shaderIDChanged)
  Q_PROPERTY(three::quick::Three::Side side READ side WRITE setSide NOTIFY sideChanged)
  Q_PROPERTY(bool depthTest READ depthTest WRITE setDepthTest NOTIFY depthTestChanged)
  Q_PROPERTY(bool depthWrite READ depthWrite WRITE setDepthWrite NOTIFY depthWriteChanged)
  Q_PROPERTY(QVariantMap uniforms READ uniforms WRITE setUniforms NOTIFY uniformsChanged)

  QString _shaderID;
  Three::Side _side = Three::FrontSide;
  bool _depthTest = true;
  bool _depthWrite = true;
  QVariantMap _uniforms;

  three::ShaderMaterial::Ptr _material;

protected:
  virtual three::ShaderMaterial::Ptr createMaterial() const;

public:
  ShaderMaterial(QObject *parent = nullptr) : Material(parent) {}

  QString shaderID() const {return _shaderID;}
  Three::Side side() const {return _side;}
  bool depthTest() const {return _depthTest;}
  bool depthWrite() const {return _depthWrite;}
  QVariantMap &uniforms() {return _uniforms;}

  void setShaderID(QString shaderID) {
    if(_shaderID != shaderID) {
      _shaderID = shaderID;
      emit shaderIDChanged();
    }
  }
  void setSide(Three::Side side) {
    if(_side != side) {
      _side = side;
      emit sideChanged();
    }
  }
  void setDepthTest(bool depthTest) {
    if(_depthTest != depthTest) {
      _depthTest = depthTest;
      emit depthTestChanged();
    }
  }
  void setDepthWrite(bool depthWrite) {
    if(_depthWrite != depthWrite) {
      _depthWrite = depthWrite;
      emit depthWriteChanged();
    }
  }
  void setUniforms(QVariantMap uniforms) {
    if(_uniforms != uniforms) {
      _uniforms = uniforms;
      emit uniformsChanged();
    }
  }

  three::ShaderMaterial::Ptr getMaterial() {
    if(!_material) _material = createMaterial();
    return _material;
  }

  void addTo(ObjectRootContainer *container) override
  {
    container->addMaterial(this);
  }

  void identify(MeshCreator &creator) override
  {
    creator.material(getMaterial());
  }

signals:
  void shaderIDChanged();
  void sideChanged();
  void depthTestChanged();
  void depthWriteChanged();
  void uniformsChanged();
};

}
}

#endif //THREEPP_SHADERMATERIAL_H
