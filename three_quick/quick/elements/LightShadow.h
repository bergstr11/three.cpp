//
// Created by byter on 12/20/17.
//

#ifndef THREEPP_LIGHTSHADOW_H
#define THREEPP_LIGHTSHADOW_H

#include <QObject>
#include <QSize>
#include <quick/cameras/PerspectiveCamera.h>

namespace three {
namespace quick {

class LightShadow : public QObject
{
Q_OBJECT
  Q_PROPERTY(QSize mapSize READ mapSize WRITE setMapSize NOTIFY mapSizeChanged)
  Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(PerspectiveCamera * camera READ camera CONSTANT)

  QSize _mapSize {512, 512};
  qreal _radius = 1;

  PerspectiveCamera _camera;

public:
  PerspectiveCamera *camera() {
    return &_camera;
  }

  QSize mapSize() {return _mapSize;}

  void setMapSize(QSize size) {
    if(_mapSize != size) {
      _mapSize = size;
      emit mapSizeChanged();
    }
  }

  qreal radius() {return _radius;}

  void setRadius(qreal radius) {
    if(_radius != radius) {
      _radius = radius;
      emit radiusChanged();
    }
  }

signals:
  void mapSizeChanged();
  void radiusChanged();
};

}
}

#endif //THREEPP_LIGHTSHADOW_H
