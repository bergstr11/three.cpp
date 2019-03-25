//
// Created by byter on 12/20/17.
//

#ifndef THREEPPQ_LIGHTSHADOW_H
#define THREEPPQ_LIGHTSHADOW_H

#include <QObject>
#include <QSize>
#include <threepp/quick/cameras/PerspectiveCamera.h>
#include <threepp/quick/cameras/OrthographicCamera.h>

namespace three {
namespace quick {

class LightShadow : public QObject
{
Q_OBJECT
  Q_PROPERTY(QSize mapSize READ mapSize WRITE setMapSize NOTIFY mapSizeChanged)
  Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(float bias READ bias WRITE setBias NOTIFY biasChanged)

  QSize _mapSize {512, 512};
  float _radius = 1;
  float _bias = 0;

public:
  LightShadow(QObject *parent=nullptr) : QObject(parent) {}

  LightShadow(const three::LightShadow &shadow, QObject *parent = nullptr)
     : QObject(parent), _radius(shadow.radius()), _bias(shadow.bias())
  {
    _mapSize.setWidth(shadow.mapSize().x());
    _mapSize.setHeight(shadow.mapSize().y());
  }

  QSize mapSize() {return _mapSize;}

  void setMapSize(QSize size) {
    if(_mapSize != size) {
      _mapSize = size;
      emit mapSizeChanged();
    }
  }

  float radius() {return _radius;}

  void setRadius(float radius) {
    if(_radius != radius) {
      _radius = radius;
      emit radiusChanged();
    }
  }

  float bias() {return _bias;}

  void setBias(float bias) {
    if(_bias != bias) {
      _bias = bias;
      emit biasChanged();
    }
  }

signals:
  void mapSizeChanged();
  void radiusChanged();
  void biasChanged();
};

class LightShadowPC : public LightShadow
{
Q_OBJECT
  Q_PROPERTY(PerspectiveCamera *camera READ camera CONSTANT)

  PerspectiveCamera _camera;

public:
  LightShadowPC(QObject *parent=nullptr) : LightShadow(parent) {}

  LightShadowPC(float fov, float aspect, float near, float far, QObject *parent = nullptr)
     : LightShadow(parent), _camera(fov, aspect, near, far)
  {}

  LightShadowPC(const three::PointLightShadow &shadow, QObject *parent = nullptr)
     : LightShadow(shadow, parent), _camera(shadow.camera_t()) {}

  PerspectiveCamera *camera() {
    return &_camera;
  }
};

class LightShadowOC : public LightShadow
{
Q_OBJECT
  Q_PROPERTY(OrthographicCamera *camera READ camera CONSTANT)

  OrthographicCamera _camera;

public:
  OrthographicCamera *camera() {
    return &_camera;
  }
};

}
}

#endif //THREEPPQ_LIGHTSHADOW_H
