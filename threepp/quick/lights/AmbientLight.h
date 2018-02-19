//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_AMBIENTLIGHT_H
#define THREEQUICK_AMBIENTLIGHT_H

#include <threepp/light/AmbientLight.h>
#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/lights/Light.h>

namespace three {
namespace quick {

class AmbientLight : public Light
{
Q_OBJECT
  three::AmbientLight::Ptr _light;
  Scene *_scene = nullptr;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _scene = scene;
    _light = three::AmbientLight::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _intensity);

    QObject::connect(this, &Light::intensityChanged, [this]() {_scene->scene()->updateMaterials();});
    return _light;
  }

  three::Light::Ptr light() override {return _light;}

public:
  AmbientLight(QObject *parent = nullptr) : Light(parent) {}

  AmbientLight(three::AmbientLight::Ptr light, QObject *parent = nullptr)
     : Light(light, parent), _light(light) {}
};

}
}

#endif //THREEQUICK_AMBIENTLIGHT_H
