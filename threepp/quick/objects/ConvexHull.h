//
// Created by byter on 5/6/18.
//

#ifndef THREE_PP_CONVEXHULL_H
#define THREE_PP_CONVEXHULL_H

#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/quick/scene/Scene.h>
#include <threepp/material/MeshBasicMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/objects/Mesh.h>
#include <threepp/util/QuickHull.h>

namespace three {
namespace quick {

class ConvexHull : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(ThreeQObject *hulledObject READ hulledObject WRITE setHulledObject NOTIFY hulledObjectChanged)
  Q_CLASSINFO("DefaultProperty", "hulledObject")

  MeshCreatorG<BufferGeometry> _creator {"convexHull"};

  ThreeQObject *_hulledObject = nullptr;
  OnObjectSetConnection hulledConnection = nullptr;

protected:
  three::Object3D::Ptr _create() override
  {
    BufferGeometry::Ptr geometry;

    if(_hulledObject) {
      _hulledObject->create(_scene, nullptr);
    }

    return nullptr;
  }

  void updateMaterial() override {
    material()->identify(_creator);
  }

  void updateHulled(Object3D::Ptr prev, Object3D::Ptr updated)
  {
    if(prev) _parentObject->remove(prev);
    if(_object) _parentObject->remove(_object);

    if(updated) {
      _object = updated;
      _parentObject->add(_object);

      updated->updateMatrix();
      updated->updateMatrixWorld(true);

      QuickHull hull(_object);
      //geometry = hull.createGeometry();

      //_creator.set(geometry);
      //material()->identify(_creator);

      //_parentObject->add(_creator.mesh);
    }
  }

public:
  ConvexHull(QObject *parent = nullptr) : ThreeQObject(parent) {}

  ThreeQObject *hulledObject() const {return _hulledObject;};

  void setHulledObject(ThreeQObject *hulledObject)
  {
    if(_hulledObject != hulledObject) {
      if(hulledConnection) {
        _hulledObject->onObjectSet.disconnect(hulledConnection);
        hulledConnection = nullptr;
      }
      _hulledObject = hulledObject;

      if(_parentObject) {
        auto hulled = _hulledObject->create(_scene, nullptr);
        if(hulled) {
          updateHulled(nullptr, hulled);
        }
      }
      else
        hulledConnection = _hulledObject->onObjectSet.connect(*this, &ConvexHull::updateHulled);

      emit hulledObjectChanged();
    }
  }

signals:
  void hulledObjectChanged();
};

}
}
#endif //THREE_PP_CONVEXHULL_H
