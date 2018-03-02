//
// Created by byter on 2/26/18.
//

#ifndef THREE_PPQ_GROUP_H
#define THREE_PPQ_GROUP_H

#include <QQmlListProperty>
#include <threepp/quick/scene/Scene.h>
#include <threepp/objects/Node.h>

namespace three {
namespace quick {

class Group : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(QQmlListProperty<three::quick::ThreeQObject> objects READ objects)
  Q_CLASSINFO("DefaultProperty", "objects")

  Scene * _scene;
  QList<ThreeQObject *> _objects;

  Node::Ptr _node;

  static void append_object(QQmlListProperty<ThreeQObject> *list, ThreeQObject *obj);
  static int count_objects(QQmlListProperty<ThreeQObject> *);
  static ThreeQObject *object_at(QQmlListProperty<ThreeQObject> *, int);
  static void clear_objects(QQmlListProperty<ThreeQObject> *);

  QQmlListProperty<ThreeQObject> objects();

protected:
  three::Object3D::Ptr _create(Scene *scene) override;

public:
  explicit Group(Scene *scene=nullptr);
};

}
}

#endif //THREE_PPQ_GROUP_H
