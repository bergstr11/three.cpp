//
// Created by byter on 3/16/18.
//

#ifndef THREE_PP_OBJECTPICKER_H
#define THREE_PP_OBJECTPICKER_H

#include <QObject>
#include <QVariantList>
#include <vector>
#include <threepp/quick/elements/RayCaster.h>
#include <threepp/quick/cameras/Camera.h>
#include <threepp/quick/objects/ThreeQObject.h>
#include "Controller.h"

namespace three {
namespace quick {

class ThreeDItem;
class Scene;

class ObjectPicker : public Controller
{
Q_OBJECT
  Q_PROPERTY(RayCaster *raycaster READ raycaster CONSTANT)
  Q_PROPERTY(QVariantList objects READ objects WRITE setObjects NOTIFY objectsChanged)
  Q_PROPERTY(bool recurse READ recurse WRITE setRecurse NOTIFY recurseChanged)
  Q_PROPERTY(unsigned intersectCount READ intersectCount NOTIFY intersectCountChanged)
  Q_PROPERTY(ThreeQObject *prototype READ prototype WRITE setPrototype NOTIFY prototypeChanged)

  ThreeDItem *_item = nullptr;
  RayCaster _raycaster;

  int _lastX=-1, _lastY=-1;

  Scene *_scene = nullptr;

  bool _recurse = true;

  bool _enabled = true;

  std::vector<Intersection> _intersects;

  std::vector<ThreeQObject *> _threeQObjects;

  Intersect _currentIntersect;
  ThreeQObject *_prototype;

public:
  explicit ObjectPicker(QObject *parent = nullptr);

  ~ObjectPicker();

  RayCaster *raycaster() {return &_raycaster;}

  void setItem(ThreeDItem *item) override;

  bool handleMousePressed(QMouseEvent *event) override;
  bool handleMouseDoubleClicked(QMouseEvent *event) override;

  Q_INVOKABLE QVariant intersect(unsigned index);

protected:
  unsigned intersectCount() {return _intersects.size();}

  QVariantList objects();

  void setObjects(QVariantList list);

  bool recurse() {return _recurse;}

  void setRecurse(bool recurse)
  {
    if(_recurse != recurse) {
      _recurse = recurse;
      emit recurseChanged();
    }
  }

  ThreeQObject *prototype() {return _prototype;}

  void setPrototype(ThreeQObject *prototype)
  {
    if(_prototype != prototype) {
      _prototype = prototype;
      emit prototypeChanged();
    }
  }

  bool enabled() override {
    return _enabled;
  }
  void setEnabled(bool enabled) override
  {
    _enabled = enabled;
  }

signals:
  void objectsChanged();
  void recurseChanged();
  void intersectCountChanged();
  void prototypeChanged();

  void objectsClicked();
  void objectsDoubleClicked();
};

}
}


#endif //THREE_PP_OBJECTPICKER_H
