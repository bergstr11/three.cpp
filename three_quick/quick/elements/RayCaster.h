//
// Created by byter on 1/29/18.
//

#ifndef THREEPP_RAYCASTER_H
#define THREEPP_RAYCASTER_H

#include <QVector2D>
#include <core/Raycaster.h>
#include <quick/cameras/Camera.h>
#include "../ThreeQObjectRoot.h"

namespace three {
namespace quick {

class RayCaster : public ThreeQObjectRoot
{
Q_OBJECT
  three::Raycaster raycaster;

public:
  Q_INVOKABLE void setFromCamera(const QVector2D &mouse, Camera *camera);
  Q_INVOKABLE QList<ThreeQObject *> intersectObjects(const QList<ThreeQObject *> &objects);

  void addTo(ObjectRootContainer *container) override;
};

}
}


#endif //THREEPP_RAYCASTER_H
