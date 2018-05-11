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

  ThreeQObject *_hulled = nullptr;
  OnObjectSetConnection _hulledConnection = nullptr;

protected:
  three::Object3D::Ptr _create() override
  {
    BufferGeometry::Ptr geometry;

    if(_hulled) {
      _hulled->create(_scene, nullptr);
    }

    return nullptr;
  }

  void updateMaterial() override {
    material()->identify(_creator);
  }

  void updateHulled(Object3D::Ptr prev, Object3D::Ptr updated)
  {
    if(prev) _parentObject->remove(prev);
    if(_hulled && _hulled->object()) _parentObject->remove(_hulled->object());

    if(updated) {
      _parentObject->add(updated);

      updated->updateMatrix();
      updated->updateMatrixWorld(true);

      QuickHull hull(updated);
      BufferGeometry::Ptr geometry = hull.createGeometry();

      _creator.set(geometry);
      material()->identify(_creator);

      _object = _creator.mesh;
      _object->visible() = _visible;
      updated->add(_object);
    }
  }

public:
  ConvexHull(QObject *parent = nullptr) : ThreeQObject(parent) {}

  ThreeQObject *hulledObject() const {return _hulled;};

  void setHulledObject(ThreeQObject *hulled)
  {
    if(_hulled != hulled) {
      if(_hulledConnection) {
        _hulled->onObjectSet.disconnect(_hulledConnection);
        _hulledConnection = nullptr;
      }
      _hulled = hulled;

      if(_parentObject) {
        auto hulledObject = _hulled->create(_scene, nullptr);
        if(hulledObject) {
          updateHulled(nullptr, hulledObject);
        }
      }
      else
        _hulledConnection = _hulled->onObjectSet.connect(*this, &ConvexHull::updateHulled);

      emit hulledObjectChanged();
    }
  }

signals:
  void hulledObjectChanged();
};

}
}
#endif //THREE_PP_CONVEXHULL_H
