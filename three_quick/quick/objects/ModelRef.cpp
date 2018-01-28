//
// Created by byter on 1/12/18.
//

#include <QDebug>
#include "ModelRef.h"
#include <quick/objects/Mesh.h>
#include <quick/objects/AmbientLight.h>
#include <quick/objects/HemisphereLight.h>
#include <quick/objects/DirectionalLight.h>
#include <quick/objects/SpotLight.h>

namespace three {
namespace quick {

void ModelRef::setModel(Model *model) {
  if(_model != model) {
    if(_loadedConnection) QObject::disconnect(_loadedConnection);
    if(_fileConnection) QObject::disconnect(_fileConnection);
    _model = model;
    if(_model) {
      _fileConnection = QObject::connect(_model, &Model::fileChanged, this, &ModelRef::cleanupMesh);
      _loadedConnection = QObject::connect(_model, &Model::modelLoaded, this, &ModelRef::updateMesh);
    }
    emit modelChanged();
  }
}

void ModelRef::cleanupMesh()
{
  for(const auto &obj : _objects) {
    _scene->scene()->remove(obj->object());
  }
  _objects.clear();
}

void ModelRef::matchType(Object3D::Ptr parent, Object3D::Ptr obj, bool setObject)
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
        if(setObject)
          _object = new three::quick::Mesh(m, this);
      }
      break;
    }
    case Light: {
      auto hl = dynamic_pointer_cast<three::HemisphereLight>(obj);
      if(hl) {
        parent->add(hl);
        if(setObject) _object = new three::quick::HemisphereLight(hl, this);
      }
      else {
        auto dl = dynamic_pointer_cast<three::DirectionalLight>(obj);
        if(dl) {
          parent->add(dl);
          if(setObject) _object = new three::quick::DirectionalLight(dl, this);
        }
        else {
          auto al = dynamic_pointer_cast<three::AmbientLight>(obj);
          if(al) {
            parent->add(al);
            if(setObject) _object = new three::quick::AmbientLight(al, this);
          }
          else {
            auto sl = dynamic_pointer_cast<three::SpotLight>(obj);
            if(sl) {
              parent->add(sl);
              if(setObject) _object = new three::quick::SpotLight(sl, this);
            }
          }
        }
      }
      break;
    }
    case Camera: {
      auto pcam = dynamic_pointer_cast<three::PerspectiveCamera>(obj);
      if (pcam) {
        _scene->scene()->add(pcam);
        if(setObject) _object = new three::quick::PerspectiveCamera(pcam, this);
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
            matchType(parent, chld, true);
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
        matchType(parent, chld, false);

        bool ret = evaluateSelector(iter, end, parent, chld->children(), ev);
        if(ret) return true;
      }
      break;
    }
  }
  return false;
}

void ModelRef::setRotateX(float angle) {
  if(_rotateX != angle) {
    float diff = angle - _rotateX;
    _rotateX = angle;
    if(_node) _node->rotateX(diff);
    emit rotateChanged();
  }
}
void ModelRef::setRotateY(float angle) {
  if(_rotateY != angle) {
    float diff = angle - _rotateY;
    _rotateY = angle;
    if(_node) _node->rotateY(diff);
    emit rotateChanged();
  }
}
void ModelRef::setRotateZ(float angle) {
  if(_rotateZ != angle) {
    float diff = angle - _rotateZ;
    _rotateZ = angle;
    if(_node) _node->rotateZ(diff);
    emit rotateChanged();
  }
}

void ModelRef::setTranslateZ(float distance) {
  if(_translateZ != distance) {
    float diff = distance - _translateZ;
    _translateZ = distance;
    if(_node) _node->translateZ(diff);
    emit translateZChanged();
  }
}

void ModelRef::updateMesh()
{
  if(_node) _scene->scene()->remove(_node);

  _node = three::Node::make(_model->name().toStdString());
  _node->rotateX(_rotateX);
  _node->rotateY(_rotateY);
  _node->rotateZ(_rotateZ);

  _node->translateZ(_translateZ);

  if(!_selector.isEmpty()) {
    QStringList selectors = _selector.split(':', QString::SkipEmptyParts);
    _objects.clear();

    auto begin = selectors.begin();
    auto end = selectors.end();

    evaluateSelector(begin, end, _node, _model->scene()->children());
  }
  else {
    //must copy, as the children are in effect moved
    const std::vector<Object3D::Ptr> children = _model->scene()->children();

    for(Object3D::Ptr child : children)  _node->add(child);
  }

  _scene->scene()->add(_node);
  if(_object)
    emit modelObjectChanged();

  emit _scene->sceneChanged();
}

}
}
