//
// Created by byter on 9/18/18.
//

#ifndef THREEPP_XAMPL_SHADOWMAPVIEWER_H
#define THREEPP_XAMPL_SHADOWMAPVIEWER_H

#include <threepp/quick/ThreeDItem.h>
#include <threepp/quick/ThreeQObjectRoot.h>
#include <QPointF>
#include <threepp/quick/lights/Light.h>
#include <threepp/helper/ShadowMapViewer.h>

namespace three {
namespace quick {

class ShadowMapViewer : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
  Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(Light *light READ light WRITE setLight NOTIFY lightChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

  three::ShadowMapViewer::Ptr _viewer;
  float _scale;
  QPointF _position;
  Light *_light = nullptr;
  bool _enabled = true;

  float scale() const {return _scale;}
  const QPointF &position() const {return _position;}
  Light *light() const {return _light;}
  bool enabled() const {return _enabled;}

  void setEnabled(bool enabled) {
    if(_enabled != enabled) {
      _enabled = enabled;
      if(_viewer) _viewer->enabled = enabled;
      emit enabledChanged();
    }
  }

  void setScale(float scale)
  {
    if(_scale != scale) {
      _scale = scale;
      emit scaleChanged();
    }
  }
  void setPosition(const QPointF &position)
  {
    if(_position != position) {
      _position = position;
      emit positionChanged();
    }
  }
  void setLight(Light *light)
  {
    if(_light != light) {
      _light = light;
      emit lightChanged();
    }
  }

  void render(OpenGLRenderer::Ptr renderer, three::Renderer::Target::Ptr target)
  {
    _viewer->render(renderer, target);
  }

  void setItem(ThreeDItem * item) override
  {
    _viewer = three::ShadowMapViewer::make(_light->light(), _position, _scale, item->width(), item->height());
    _viewer->enabled = _enabled;
    item->onRendered.connect(this, &ShadowMapViewer::render);
  }

public:
  ShadowMapViewer(QObject *parent=nullptr) : ThreeQObjectRoot(parent)
  {}

signals:
  void scaleChanged();
  void positionChanged();
  void lightChanged();
  void enabledChanged();
};

}
}

#endif //THREEPP_XAMPL_SHADOWMAPVIEWER_H
