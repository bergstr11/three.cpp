//
// Created by byter on 2/26/18.
//

#include "Group.h"

namespace three {
namespace quick {

Group::Group(Scene *scene) : ThreeQObject(scene), _scene(scene) {}

three::Object3D::Ptr Group::_create(Scene *scene)
{
  _node = Node::make(_name.toStdString());
  for(auto obj : _objects) {
    _node->add(obj->create(scene));
  }
  return _node;
}

void Group::append_object(QQmlListProperty<ThreeQObject> *list, ThreeQObject *obj)
{
  Group *item = qobject_cast<Group *>(list->object);
  if (item) {
    item->_objects.append(obj);
    if(item->_node) {
      item->_node->add(obj->create(item->_scene));
    }
  }
}
int Group::count_objects(QQmlListProperty<ThreeQObject> *list)
{
  Group *item = qobject_cast<Group *>(list->object);
  return item ? item->_objects.size() : 0;
}
ThreeQObject *Group::object_at(QQmlListProperty<ThreeQObject> *list, int index)
{
  Group *item = qobject_cast<Group *>(list->object);
  return item ? item->_objects.at(index) : nullptr;
}
void Group::clear_objects(QQmlListProperty<ThreeQObject> *list)
{
  Group *item = qobject_cast<Group *>(list->object);
  if(item) item->_objects.clear();
  if(item->_node) {
    item->_node->removeAll();
  }
}

QQmlListProperty<ThreeQObject> Group::objects()
{
  return QQmlListProperty<ThreeQObject>(this, nullptr,
                                        &Group::append_object,
                                        &Group::count_objects,
                                        &Group::object_at,
                                        &Group::clear_objects);
}

}
}