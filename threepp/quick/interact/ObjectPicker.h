//
// Created by byter on 3/16/18.
//

#ifndef THREE_PP_OBJECTPICKER_H
#define THREE_PP_OBJECTPICKER_H

#include <QObject>
#include <QVariantList>
#include <vector>
#include <threepp/quick/cameras/Camera.h>
#include <threepp/quick/ThreeQObjectRoot.h>
#include <threepp/quick/ThreeDItem.h>
#include <threepp/quick/elements/RayCaster.h>

namespace three {
namespace quick {

class Scene;
class Camera;

class ObjectPicker;

class Pickable : public QObject
{
  Q_OBJECT
  Object3D::Ptr _object;

public:
  explicit Pickable(Object3D::Ptr object, QObject *parent=nullptr) : QObject(parent), _object(object) {}

  Object3D::Ptr object() const {return _object;}
};

/**
 * abstract ray configuration class
 */
class Rays : public QObject
{
Q_OBJECT

protected:
  math::Vector3 _origin;
  math::Vector3 _surfacePosition, _surfaceNormal;

  Object3D *_picked = nullptr;

public:
  const math::Vector3 origin() const {return _origin;}
  const math::Vector3 surfacePosition() const {return _surfacePosition;}
  const math::Vector3 surfaceNormal() const {return _surfaceNormal;}

  virtual Raycaster raycaster(const math::Ray &cameraRay) = 0;

  virtual void setIntersects(IntersectList &intersects) = 0;

  virtual void scaleTo(float radius) {}

  virtual bool accept(const IntersectList &list) const = 0;

  Object3D *picked() {return _picked;}
};

/**
 * single ray picker configuration
 */
class SingleRay : public Rays
{
Q_OBJECT

public:

  Raycaster raycaster(const math::Ray &cameraRay) override;

  void setIntersects(IntersectList &intersects) override;

  bool accept(const IntersectList &list) const override {return !list.empty();}
};

/**
 * circular multi-ray picker configuration
 */
class CircularRays : public Rays
{
Q_OBJECT
  Q_PROPERTY(float radius MEMBER _radius NOTIFY radiusChanged)
  Q_PROPERTY(unsigned segments READ segments WRITE setSegments NOTIFY segmentsChanged)

  float _radius = 20, _scaleTo = 100;

  unsigned _segments = 12;

  unsigned segments() const {return _segments;}

  void setSegments(unsigned segments);

public:
  Raycaster raycaster(const math::Ray &cameraRay) override;

  void setIntersects(IntersectList &intersects) override;

  void scaleTo(float scaleTo) override {
    _scaleTo = scaleTo;
  }

  bool accept(const IntersectList &list) const override {
    float required = (float)_segments / 1.7f;
    return list.rayCount() >= required;
  }

signals:
  void radiusChanged();
  void segmentsChanged();
};

/**
 * a picker handles mouse events and determines, whether the mouse coordinates correspond to
 * one or more objects in the 3D space. It supports different ray configurations, ranging from
 * single ray to multi-ray
 */
class ObjectPicker : public ThreeQObjectRoot, public Interactor
{
Q_OBJECT
  Q_PROPERTY(three::quick::Camera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
  Q_PROPERTY(QVariantList objects READ objects WRITE setObjects NOTIFY objectsChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(Rays *rays READ rays WRITE setRays NOTIFY raysChanged)
  Q_PROPERTY(ThreeQObject *prototype READ prototype WRITE setPrototype NOTIFY prototypeChanged)
  Q_PROPERTY(QQmlListProperty<three::quick::ObjectPicker> pickers READ pickers)
  Q_CLASSINFO("DefaultProperty", "pickers")

  ThreeDItem *_item = nullptr;
  Camera *_camera = nullptr;

  QList<ObjectPicker *> _pickers;

  int _lastX=-1, _lastY=-1;

  Scene *_scene = nullptr;

  IntersectList _intersects;

  QVariantList _objects;

  Intersect _currentIntersect;
  Rays *_rays = nullptr;

  ThreeQObject *_prototype = nullptr;

  float _scaleSize = 0;

  static void append_picker(QQmlListProperty<ObjectPicker> *list, ObjectPicker *obj);
  static int count_pickers(QQmlListProperty<ObjectPicker> *);
  static ObjectPicker *picker_at(QQmlListProperty<ObjectPicker> *, int);
  static void clear_pickers(QQmlListProperty<ObjectPicker> *);

  QQmlListProperty<ObjectPicker> pickers();

public:
  explicit ObjectPicker(QObject *parent = nullptr);

  ~ObjectPicker() override;

  void setItem(ThreeDItem *item) override;

  Object3D *pickedObject() { return _rays->picked(); }

  Camera *camera() const {return _camera;}

  void setCamera(three::quick::Camera *camera);

  bool handleMousePressed(QMouseEvent *event) override;
  bool handleMouseDoubleClicked(QMouseEvent *event) override;

  Q_INVOKABLE void scaleTo(ThreeQObject *object);

  Q_INVOKABLE QVariant intersect(unsigned index);

  float scaleSize() const {return _scaleSize;}

protected:
  QVariantList objects() {return _objects;}

  void setObjects(const QVariantList &list);

  Rays *rays() {return _rays;}

  void setRays(Rays *rays);

  ThreeQObject *prototype() {return _prototype;}

  void setPrototype(ThreeQObject *prototype)
  {
    if(_prototype != prototype) {
      _prototype = prototype;
      emit prototypeChanged();
    }
  }

  virtual bool enabled()
  {
    return _enabled;
  }

  void setEnabled(bool enabled)
  {
    if(_enabled != enabled) {
      _enabled = enabled;
      emit enabledChanged();
    }
  }

public:
  const IntersectList &intersects() const {return _intersects;}

  Rays &getRays() {return *_rays;}

signals:
  void objectsChanged();
  void prototypeChanged();
  void cameraChanged();
  void enabledChanged();
  void raysChanged();

  void objectsClicked();
  void objectsDoubleClicked();
};

}
}


#endif //THREE_PP_OBJECTPICKER_H
