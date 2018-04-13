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

class Pickable : public QObject
{
  Q_OBJECT
  Object3D::Ptr _object;

public:
  explicit Pickable(Object3D::Ptr object, QObject *parent=nullptr) : QObject(parent), _object(object) {}

  Object3D::Ptr object() const {return _object;}
};

class ObjectPicker : public Controller
{
Q_OBJECT
  Q_PROPERTY(RayCaster *raycaster READ raycaster CONSTANT)
  Q_PROPERTY(QVariantList objects READ objects WRITE setObjects NOTIFY objectsChanged)
  Q_PROPERTY(bool pickAll READ pickAll WRITE setPickAll NOTIFY pickAllChanged)
  Q_PROPERTY(unsigned intersectCount READ intersectCount NOTIFY intersectCountChanged)
  Q_PROPERTY(ThreeQObject *prototype READ prototype WRITE setPrototype NOTIFY prototypeChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ObjectPicker> pickers READ pickers)
  Q_CLASSINFO("DefaultProperty", "pickers")

  ThreeDItem *_item = nullptr;
  RayCaster _raycaster;

  QList<ObjectPicker *> _pickers;

  int _lastX=-1, _lastY=-1;

  Scene *_scene = nullptr;

  bool _pickAll = true;

  bool _enabled = true;

  std::vector<Intersection> _intersects;

  QVariantList _objects;

  Intersect _currentIntersect;
  ThreeQObject *_prototype;

  static void append_picker(QQmlListProperty<ObjectPicker> *list, ObjectPicker *obj);
  static int count_pickers(QQmlListProperty<ObjectPicker> *);
  static ObjectPicker *picker_at(QQmlListProperty<ObjectPicker> *, int);
  static void clear_pickers(QQmlListProperty<ObjectPicker> *);

  QQmlListProperty<ObjectPicker> pickers();

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

  QVariantList objects() {return _objects;}

  void setObjects(const QVariantList &list);

  bool pickAll() {return _pickAll;}

  void setPickAll(bool pickAll)
  {
    if(_pickAll != pickAll) {
      _pickAll = pickAll;
      emit pickAllChanged();
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
  void pickAllChanged();
  void intersectCountChanged();
  void prototypeChanged();

  void objectsClicked();
  void objectsDoubleClicked();
};

}
}


#endif //THREE_PP_OBJECTPICKER_H
