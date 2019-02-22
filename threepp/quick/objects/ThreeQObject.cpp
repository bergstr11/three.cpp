//
// Created by byter on 2/9/18.
//

#include "ThreeQObject.h"
#include <threepp/quick/scene/Scene.h>
#include <QQmlEngine>

namespace three {
namespace quick {

VertexNormalsHelper *ThreeQObject::vertexNormals()
{
  if(!_normalsHelper) {
    _normalsHelper = new VertexNormalsHelper(this);

    if(_object) {
      _normalsHelper->create(_object, _parentObject, true);
    }
  }
  return _normalsHelper;
}

BoundingBox *ThreeQObject::boundingBox()
{
  if(!_boundingBox && _object) {
    auto bbox = _object->computeBoundingBox();
    _boundingBox = new BoundingBox(bbox, this);
  }
  return _boundingBox;
}

void ThreeQObject::copy(ThreeQObject *copyable)
{
  if(_parentObject && _object) _parentObject->remove(_object);

  _object = _copy(copyable->object());

  if(_object) {
    if(_rotation.isSet())
      _object->rotation().set(_rotation().x(), _rotation().y(), _rotation().z());
    else
      _object->rotation().set(copyable->_rotation().x(), copyable->_rotation().y(), copyable->_rotation().z());

    if(_position.isSet())
      _object->position().set(_position().x(), _position().y(), _position().z());
    else
      _object->position().set(copyable->_position().x(), copyable->_position().y(), copyable->_position().z());

    if(_scale.isSet())
      _object->scale().set(_scale().x(), _scale().y(), _scale().z());
    else
      _object->scale().set(copyable->_scale().x(), copyable->_scale().y(), copyable->_scale().z());

    if(_castShadow.isSet())
      _object->castShadow = _castShadow;
    else
      _object->castShadow = copyable->_castShadow;

    if(_receiveShadow.isSet())
      _object->receiveShadow = _receiveShadow;
    else
      _object->receiveShadow = copyable->_receiveShadow;

    if(_matrixAutoUpdate.isSet())
      _object->matrixAutoUpdate = _matrixAutoUpdate;
    else
      _object->matrixAutoUpdate = copyable->_matrixAutoUpdate;

    if(_visible.isSet())
      _object->visible() = _visible;
    else
      _object->visible() = copyable->_visible;

    if(!_name.isEmpty()) _object->setName(_name.toStdString());
    if(_parentObject) _parentObject->add(_object);

    onObjectChanged.emitSignal(_object, ObjectState::Added);
    _object->updateMatrix();
  }
}

three::Object3D::Ptr ThreeQObject::create(Scene *scene, Object3D::Ptr parent)
{
  _scene = scene;
  _parentObject = parent;

  _object = _create();

  if(_object) {
    if(!_rotation().isNull())
      _object->rotation().set(_rotation().x(), _rotation().y(), _rotation().z());

    _object->position().set(_position().x(), _position().y(), _position().z());

    _object->scale().set(_scale().x(), _scale().y(), _scale().z());

    _object->castShadow = _castShadow;
    _object->receiveShadow = _receiveShadow;
    _object->matrixAutoUpdate = _matrixAutoUpdate;
    _object->visible() = _visible;

    for(auto o : _children) {
      three::Object3D::Ptr obj = o->create(_scene, _object);
    }

    if(!_name.isEmpty()) _object->setName(_name.toStdString());

    if(_normalsHelper) _normalsHelper->create(_object, _parentObject);

    _post_create();

    if(_parentObject) _parentObject->add(_object);

    onObjectChanged.emitSignal(_object, ObjectState::Added);
    _object->updateMatrix();

    emit objectCreated();
  }

  return _object;
}

void ThreeQObject::append_child(QQmlListProperty<ThreeQObject> *list, ThreeQObject *obj)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  if (item) item->_children.append(obj);
}
int ThreeQObject::count_children(QQmlListProperty<ThreeQObject> *list)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  return item ? item->_children.size() : 0;
}
ThreeQObject *ThreeQObject::child_at(QQmlListProperty<ThreeQObject> *list, int index)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  return item ? item->_children.at(index) : nullptr;
}
void ThreeQObject::clear_children(QQmlListProperty<ThreeQObject> *list)
{
  ThreeQObject *item = qobject_cast<ThreeQObject *>(list->object);
  if(item) item->_children.clear();
}

QQmlListProperty<ThreeQObject> ThreeQObject::children()
{
  return QQmlListProperty<ThreeQObject>(this, nullptr,
                                        &ThreeQObject::append_child,
                                        &ThreeQObject::count_children,
                                        &ThreeQObject::child_at,
                                        &ThreeQObject::clear_children);
}

void ThreeQObject::recreate()
{
  if(_parentObject) {
    if(_object) _parentObject->remove(_object);
    create(_scene, _parentObject);
  }
}

void ThreeQObject::init()
{
  if(_object) {

    const math::Vector3 pos = _object->position();
    setPosition(QVector3D(pos.x(), pos.y(), pos.z()), false);
    _position.unset();

    const math::Euler rot = _object->rotation();
    setRotation(QVector3D(rot.x(), rot.y(), rot.z()), false);
    _rotation.unset();

    setName(QString::fromStdString(_object->name()), false);

    setCastShadow(_object->castShadow, false);

    setReceiveShadow(_object->castShadow, false);

    const math::Vector3 &s = _object->scale();
    setScale(QVector3D(s.x(), s.y(), s.z()), false);
    _scale.unset();
  }
}

void ThreeQObject::setObject(const three::Object3D::Ptr &object)
{
  _object = object;
  if(_object) {

    if(_position.isSet()) {
      object->position().set(_position().x(), _position().y(), _position().z());
    }
    else {
      const math::Vector3 pos = _object->position();
      setPosition(QVector3D(pos.x(), pos.y(), pos.z()), false);
      _position.unset();
    }

    if(_rotation.isSet()) {
      object->rotation().set(_rotation().x(), _rotation().y(), _rotation().z());
    }
    else {
      const math::Euler rot = _object->rotation();
      setRotation(QVector3D(rot.x(), rot.y(), rot.z()), false);
      _rotation.unset();
    }

    setName(QString::fromStdString(_object->name()), false);

    if(_castShadow.isSet()) _object->castShadow = _castShadow;
    else setCastShadow(_object->castShadow, false);

    if(_receiveShadow.isSet()) _object->receiveShadow = _receiveShadow;
    else setReceiveShadow(_object->castShadow, false);

    if(_visible.isSet()) _object->visible() = _visible;
    if(_matrixAutoUpdate.isSet()) _object->matrixAutoUpdate = _matrixAutoUpdate;

    if(_scale.isSet()) {
      _object->scale().set(_scale().x(), _scale().y(), _scale().z());
    }
    else {
      const math::Vector3 &s = _object->scale();
      setScale(QVector3D(s.x(), s.y(), s.z()), false);
      _scale.unset();
    }

    if(_normalsHelper) {
      _normalsHelper->setObject(_object, _parentObject);
    }
  }
}

void ThreeQObject::add(ThreeQObject *object)
{
  if(_object) {
    auto obj = object->object();
    if(!obj) obj = object->create(_scene, _object);
    if(obj) {
      _object->add(obj);
      obj->updateMatrix();
    }
  }
}

void ThreeQObject::remove(ThreeQObject *object)
{
  if(_object && object->object()) {
    _object->remove(object->object());
  }
}

void ThreeQObject::clear()
{
  if(_object) {
    _object->removeAll();
  }
}

void ThreeQObject::unset()
{
  _position.unset();
  _rotation.unset();
  _castShadow.unset();
  _receiveShadow.unset();
  _scale.unset();
  _visible.unset();
  _matrixAutoUpdate.unset();
}

void ThreeQObject::rotateX(float angle)
{
  if(_object) {
    _object->rotateX(angle);
    _rotation().setX(_object->rotation().x());
    emit rotationChanged();
  }
}
void ThreeQObject::rotateY(float angle)
{
  if(_object) {
    _object->rotateY(angle);
    _rotation().setY(_object->rotation().y());
    emit rotationChanged();
  }
}
void ThreeQObject::rotateZ(float angle)
{
  if(_object) {
    _object->rotateZ(angle);
    _rotation().setZ(_object->rotation().z());
    emit rotationChanged();
  }
}

void ThreeQObject::translateZ(float distance)
{
  if(_object) {
    _object->translateZ(distance);
    _position().setZ(_object->position().z());
    emit positionChanged();
  }
}

void ThreeQObject::lookAt(const QVector3D &position)
{
  if(_object) {
    _object->lookAt(math::Vector3(position.x(), position.y(), position.z()));

    const math::Euler rot = _object->rotation();
    setRotation(QVector3D(rot.x(), rot.y(), rot.z()), false);
  }
}

QVector3D ThreeQObject::worldPosition() const
{
  const math::Vector3 pos = _object ? _object->getWorldPosition() : math::Vector3();
  return QVector3D(pos.x(), pos.y(), pos.z());
}

QVariant ThreeQObject::parentObject(QString name)
{
  Object3D *parent = nullptr;
  std::string stdname = name.toStdString();

  if(_object) {
    parent = _object->parent();
    while(parent && parent->name() != stdname)
      parent = parent->parent();
  }
  QVariant var;
  if(parent) var.setValue(new ThreeQObject(Object3D::Ptr (parent, _no_delete)));

  return var;
}

void ThreeQObject::setPosition(const QVector3D &position, bool propagate) {
  if(position != _position) {
    _position = position;

    if(propagate && _object) _object->position().set(_position().x(), _position().y(), _position().z());

    emit positionChanged();
  }
}

void ThreeQObject::setRotation(const QVector3D &rotation, bool propagate) {
  if(_rotation != rotation) {
    _rotation = rotation;

    if(propagate && _object) {
      _object->rotation().set(_rotation().x(), _rotation().y(), _rotation().z());
    }

    emit rotationChanged();
  }
}

void ThreeQObject::setScale(QVector3D scale, bool propagate) {
  if(_scale != scale) {
    _scale = scale;
    if(propagate && _object) _object->scale().set(scale.x(), scale.x(), scale.z());
    emit scaleChanged();
  }
}

void ThreeQObject::setMaterial(Material *material, bool update) {
  if(_material != material) {
    _material = material;
    if(_object && update) updateMaterial();
    emit materialChanged();
  }
}

void ThreeQObject::setMatrixAutoUpdate(bool matrixAutoUpdate, bool propagate) {
  if(this->matrixAutoUpdate() != matrixAutoUpdate) {
    _matrixAutoUpdate = matrixAutoUpdate;
    if(propagate && _object) _object->matrixAutoUpdate = _matrixAutoUpdate;
    emit matrixAutoUpdateChanged();
  }
}

void ThreeQObject::setCastShadow(bool castShadow, bool propagate) {
  if(this->castShadow() != castShadow) {
    _castShadow = castShadow;
    if(propagate && _object) {
      _object->visit([this](Object3D *o) {o->castShadow = _castShadow; return true;});
    }
    emit castShadowChanged();
  }
}

void ThreeQObject::setReceiveShadow(bool receiveShadow, bool propagate) {
  if(this->receiveShadow() != receiveShadow) {
    _receiveShadow = receiveShadow;
    if(propagate && _object) {
      _object->visit([this](Object3D *o) {o->receiveShadow = _receiveShadow; return true;});
    }
    emit receiveShadowChanged();
  }
}

void ThreeQObject::setVisible(bool visible, bool propagate) {
  if(this->visible() != visible) {
    _visible = visible;

    if(propagate && _object) _object->visible() = _visible;

    emit visibleChanged();
  }
}

QObject *ThreeQObject::alone()
{
  ThreeQObject *alone = new ThreeQObject(_object);
  QQmlEngine::setObjectOwnership(alone, QQmlEngine::JavaScriptOwnership);
  return alone;
}

void ThreeQObject::setName(const QString &name, bool propagate)
{
  if(_name != name) {
    _name = name;
    if(_object && propagate) _object->setName(_name.toStdString());

    emit nameChanged();
  }
}

void ThreeQObject::setGeometryType(Three::GeometryType geometryType) {
  if(_geometryType != geometryType) {
    _geometryType = geometryType;
    emit geometryTypeChanged();
  }
}

}
}