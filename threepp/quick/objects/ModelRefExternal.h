//
// Created by byter on 3/18/18.
//

#ifndef THREE_PP_MODELREFEXTERNAL_H
#define THREE_PP_MODELREFEXTERNAL_H

#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/objects/ThreeQObject.h>

namespace three {
namespace quick {

class ModelRefExternal : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(ThreeQObject *object READ object WRITE setObject NOTIFY objectChanged)
  Q_PROPERTY(bool replace READ replace WRITE setReplace NOTIFY replaceChanged)

  bool _replace = true;
  Scene *_scene = nullptr;

  ThreeQObject *_threeQObject = nullptr;

protected:
  ModelRefExternal(QObject *parent=nullptr) : ThreeQObject(parent) {}

  Object3D::Ptr _create(Scene *scene) override
  {
    _scene = scene;
    return nullptr;
  }

  ThreeQObject *object() {return _threeQObject;}
  void setObject(ThreeQObject *object);

  bool replace() const {return _replace;}
  void setReplace(bool replace);

signals:
  void objectChanged();
  void replaceChanged();
};

}
}

#endif //THREE_PP_MODELREFEXTERNAL_H
