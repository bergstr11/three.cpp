//
// Created by byter on 12/29/17.
//

#ifndef THREEPPQ_THREE_H
#define THREEPPQ_THREE_H

#include <QVariant>
#include <QObject>
#include <threepp/Constants.h>

namespace three {
namespace quick {

void init();

class Three : public QObject
{
Q_OBJECT
public:
  enum GeometryType {
    DefaultGeometry,
    BufferGeometry
  };
  Q_ENUM(GeometryType)

  enum ShadowType {
    None = (unsigned)three::ShadowMapType::NoShadow,
    Basic = (unsigned)three::ShadowMapType::Basic,
    PCF  = (unsigned)three::ShadowMapType::PCF,
    PCFSoft  = (unsigned)three::ShadowMapType::PCFSoft
  };
  Q_ENUM(ShadowType)

  enum Side {
    FrontSide = (unsigned)three::Side::Front,
    BackSide = (unsigned)three::Side::Back,
    FrontAndBackSide = (unsigned)three::Side::Double
  };
  Q_ENUM(Side)

  enum CullFace
  {
    NoFaceCulling  = (unsigned)three::CullFace::None,
    BackFaceCulling = (unsigned)three::CullFace::Back,
    FrontFaceCulling = (unsigned)three::CullFace::Front,
    FrontBackFaceCulling = (unsigned)three::CullFace::FrontBack
  };
  Q_ENUM(CullFace)

  enum FrontFaceDirection
  {
    FaceDirectionCW = (unsigned)three::FrontFaceDirection::CW,
    FaceDirectionCCW = (unsigned)three::FrontFaceDirection::CCW,
    FaceDirectionUndefined = (unsigned)three::FrontFaceDirection::Undefined
  };
  Q_ENUM(FrontFaceDirection)

  Q_INVOKABLE QVariant raycaster(QVariant camera);
};

class Scene;
class Material;
class Texture;
class Controller;
class RayCaster;

class ObjectRootContainer {
public:
  virtual void addMaterial(Material *material) {}
  virtual void addTexture(Texture *texture) {}
  virtual void addController(Controller* controller) {}
  virtual void addScene(Scene *scene) {}
};

}
}


#endif //THREEPPQ_THREE_H
