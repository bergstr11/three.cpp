//
// Created by byter on 1/12/18.
//

#include "ModelRef.h"
#include <threepp/quick/objects/Mesh.h>
#include <threepp/quick/lights/AmbientLight.h>
#include <threepp/quick/lights/HemisphereLight.h>
#include <threepp/quick/lights/DirectionalLight.h>
#include <threepp/quick/lights/SpotLight.h>
#include <threepp/quick/ThreeDItem.h>

#include <threepp/quick/elements/RayCaster.h>
#include <threepp/extras/PointsWalker.h>
#include <threepp/extras/QuickHull.h>

namespace three {
namespace quick {

void ModelRef::setModel(Model *model)
{
  if(_model != model) {
    if(_loadedConnection) QObject::disconnect(_loadedConnection);
    if(_fileConnection) QObject::disconnect(_fileConnection);
    _model = model;
    if(_model) {
      _fileConnection = QObject::connect(_model, &Model::fileChanged, this, &ModelRef::cleanup);
      _loadedConnection = QObject::connect(_model, &Model::modelLoaded, this, &ModelRef::update);
    }
    emit modelChanged();
  }
}

void ModelRef::setOperation(Operation operation)
{
  if(_operation != operation) {
    _operation = operation;
    emit operationChanged();
  }
}

void ModelRef::matchType(Object3D::Ptr parent, Object3D::Ptr obj)
{
  switch (_type) {
    case Node: {
      auto o = dynamic_pointer_cast<three::Node>(obj);
      if (o) {
        parent->add(o);
      }
      break;
    }
    case Mesh: {
      auto m = dynamic_pointer_cast<three::Mesh>(obj);
      if (m) {
        parent->add(m);
      }
      break;
    }
    case Light: {
      auto hl = dynamic_pointer_cast<three::HemisphereLight>(obj);
      if(hl) {
        parent->add(hl);
      }
      else {
        auto dl = dynamic_pointer_cast<three::DirectionalLight>(obj);
        if(dl) {
          parent->add(dl);
        }
        else {
          auto al = dynamic_pointer_cast<three::AmbientLight>(obj);
          if(al) {
            parent->add(al);
          }
          else {
            auto sl = dynamic_pointer_cast<three::SpotLight>(obj);
            if(sl) {
              parent->add(sl);
            }
          }
        }
      }
      break;
    }
    case Camera: {
      auto cam = dynamic_pointer_cast<three::Camera>(obj);
      if (cam) {
        parent->add(cam);
      }
      break;
    }
  }
}

bool ModelRef::evaluateSelector(QStringList::iterator &iter,
                                QStringList::iterator &end,
                                Object3D::Ptr parent,
                                const std::vector<Object3D::Ptr> &children,
                                Eval eval)
{
  Eval ev = eval;
  if(*iter == "*") ev = Eval::skipLevel;
  else if(*iter == "+") ev = Eval::collectLevel;
  else if(*iter == "**") ev = Eval::skipLevels;
  else if(*iter == "++") ev = Eval::collectLevels;

  switch(ev) {
    case Eval::name: {
      string nm = iter->toStdString();
      bool e = ++iter == end;
      for (auto chld : children) {
        if(chld->name() == nm) {
          if (e) {
            matchType(parent, chld);
            return true;
          }
          else {
            three::Node::Ptr node = three::Node::make(chld->name());
            bool ret = evaluateSelector(iter, end, node, chld->children(), Eval::name);
            if(!node->isEmpty()) parent->add(node);
            if(ret) return true;
          }
        }
      }
      break;
    }
    case Eval::skipLevel:
      eval = Eval::name;
    case Eval::skipLevels:
      if(++iter != end) {
        for (auto chld : children) {
          if(evaluateSelector(iter, end, parent, chld->children(), eval)) return true;
        }
      }
      break;
    case Eval::collectLevel:
      ev = Eval::name;
      ++iter;
    case Eval::collectLevels: {
      for (auto chld : children) {
        matchType(parent, chld);

        bool ret = evaluateSelector(iter, end, parent, chld->children(), ev);
        if(ret) return true;
      }
      break;
    }
  }
  return false;
}

void ModelRef::cleanup()
{
  if(_operation == Replace && _object) {
    if(_parentObject) _parentObject->remove(_object);
    onObjectChanged.emitSignal(_object, ObjectState::Removed);
    _object->dispose();
  }
}

void ModelRef::update()
{
  if(_operation == Replace && _threeQObject) {
    _threeQObject->deleteLater();
    _threeQObject = nullptr;
  }

  Object3D::Ptr node = _object;
  if(_operation == Replace || !node) {
    string name = _name.isEmpty() ? _model->name().toStdString() : _name.toStdString();
    node = three::Node::make(name);
  }

  //must copy the list, as the children are in effect moved (reparented)
  const std::vector<Object3D::Ptr> children = _model->importedScene()->children();

  if(!_selector.isEmpty()) {
    QStringList selectors = _selector.split(':', QString::SkipEmptyParts);

    auto begin = selectors.begin();
    auto end = selectors.end();

    evaluateSelector(begin, end, node, children);
  }
  else {

    for(Object3D::Ptr child : children)  {
      node->add(child);
    }
  }

  //propagate shadow stuff down the tree
  node->visit([&](Object3D *o) {
    o->castShadow = _castShadow;
    o->receiveShadow = _receiveShadow;
    return true;
  });

  if(_operation == Replace) {
    setObject(node);
    if(_parentObject) _parentObject->add(_object);

    onObjectChanged.emitSignal(_object, ObjectState::Added);
    emit objectChanged();
  }

  _scene->item()->update();
}

ThreeQObject *ModelRef::getThreeQObject()
{
  if(_threeQObject) return _threeQObject;
  if(!_object) return nullptr;

  if(_object->children().size() != 1) {
    _threeQObject = new ThreeQObject(_object, this);
    return _threeQObject;
  }

  Material *material = nullptr;
  Object3D::Ptr obj = _object->children().at(0);

  three::MeshBasicMaterial::Ptr basic = dynamic_pointer_cast<three::MeshBasicMaterial>(obj->material());
  if(basic)
    material = new MeshBasicMaterial(basic);
  else {
    three::MeshLambertMaterial::Ptr lambert = dynamic_pointer_cast<three::MeshLambertMaterial>(obj->material());
    if(lambert)
      material = new MeshLambertMaterial(lambert);
    else {
      three::MeshPhongMaterial::Ptr phong = dynamic_pointer_cast<three::MeshPhongMaterial>(obj->material());
      if(phong)
        material = new MeshPhongMaterial(phong);
    }
  }
  _threeQObject = new ThreeQObject(obj, material, this);
  return _threeQObject;
}

}
}
