//
// Created by byter on 12/29/17.
//

#ifndef THREEPPQ_THREE_H
#define THREEPPQ_THREE_H

#include <QVariant>
#include <QObject>
#include <threepp/Constants.h>
#include <threepp/math/Math.h>

namespace three {
namespace quick {

void init();

class Three : public QObject
{
Q_OBJECT
public:
  enum ToneMapping {
    NoToneMapping = (unsigned)three::ToneMapping::None,
    LinearToneMapping = (unsigned)three::ToneMapping::Linear,
    ReinhardToneMapping = (unsigned)three::ToneMapping::Reinhard,
    Uncharted2ToneMapping = (unsigned)three::ToneMapping::Uncharted2,
    CineonToneMapping = (unsigned)three::ToneMapping::Cineon
  };
  Q_ENUM(ToneMapping);

  enum GeometryType {
    LinearGeometry,
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

  Q_PROPERTY(qreal DEG2RAD READ deg2rad CONSTANT)
  Q_PROPERTY(qreal RAD2DEG READ rad2deg CONSTANT)

  Q_INVOKABLE QVariant raycaster(QVariant camera);
  Q_INVOKABLE QVariant color(QString hex);
  Q_INVOKABLE QVariant randomColor(int hex);
  Q_INVOKABLE float randFloatSpread(float range);

  qreal deg2rad() {return three::math::DEG2RAD;}
  qreal rad2deg() {return three::math::RAD2DEG;}
};

}
}


#endif //THREEPPQ_THREE_H
