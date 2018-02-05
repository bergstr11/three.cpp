//
// Created by byter on 12/29/17.
//

#ifndef THREEPPQ_THREEQOBJECTROOT_H
#define THREEPPQ_THREEQOBJECTROOT_H

#include <QObject>

namespace three {
namespace quick {

class ObjectRootContainer;

class ThreeQObjectRoot : public QObject
{
Q_OBJECT

protected:
  explicit ThreeQObjectRoot(QObject *parent=nullptr) : QObject(parent) {}

public:
  virtual ~ThreeQObjectRoot() {}

  virtual void addTo(ObjectRootContainer * container) = 0;
};

}
}


#endif //THREEPPQ_THREEQOBJECTROOT_H
