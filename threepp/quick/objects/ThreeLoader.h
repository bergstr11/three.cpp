//
// Created by byter on 5/11/19.
//

#ifndef THREE_PP_THREELOADER_H
#define THREE_PP_THREELOADER_H

#include <QVariantMap>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlProperty>
#include <threepp/quick/objects/ThreeQObject.h>

namespace three {
namespace quick {

/**
 * a simple component loader for ThreeDObjects
 */
class ThreeLoader : public ThreeQObject
{
  Q_OBJECT
  Q_PROPERTY(QVariantMap properties READ properties WRITE setProperties NOTIFY propertiesChanged)
  Q_PROPERTY(QQmlComponent * component READ component WRITE setComponent NOTIFY componentChanged)
  Q_PROPERTY(ThreeQObject * item READ item NOTIFY itemChanged)

  QVariantMap _properties;
  QQmlComponent *_component = nullptr;
  ThreeQObject *_child = nullptr;

  const QVariantMap &properties() {return _properties;}

  void setProperties(const QVariantMap &props) {
    if(props != _properties) {
      _properties = props;
      emit propertiesChanged();
    }
  }

  QQmlComponent *component() {return _component;}

  void setComponent(QQmlComponent *component) {
    if(component != _component) {
      _component = component;
      emit componentChanged();
    }
  }

  ThreeQObject *item() {return _child;}

protected:

  three::Object3D::Ptr _create() override
  {
    if(_component) {
      QObject * obj = _component->beginCreate(QQmlEngine::contextForObject(this));
      if(obj) {
        for(auto it = _properties.constBegin(); it != _properties.constEnd(); it++) {
          QQmlProperty(obj, it.key()).write(it.value());
        }
        _component->completeCreate();

        _child = dynamic_cast<ThreeQObject *>(obj);
        if(!_child) {
          qWarning() << "error: component does not instantiate a ThreeQObject";
        }
        else {
          auto threeD = _child->create(_scene, _parentObject);
          if(threeD) {
            _scene->scene()->add(threeD);
          }
          emit itemChanged();
        }
      }
    }
    return nullptr;
  }

  void updateMaterial() override {
    if(_child && _child->object()) _child->object()->setMaterial(material()->getMaterial());
  }

public:
  ThreeLoader(QObject *parent = nullptr) : ThreeQObject(parent) {}

signals:
  void componentChanged();
  void itemChanged();
  void propertiesChanged();
};

}
}

#endif //THREE_PP_THREELOADER_H
