//
// Created by byter on 8/18/18.
//

#ifndef THREE_PP_DRAG_H
#define THREE_PP_DRAG_H

#include <threepp/util/osdecl.h>
#include <threepp/camera/Camera.h>
#include <threepp/math/Plane.h>
#include <threepp/util/simplesignal.h>

namespace three {
namespace control {

/*
 * @author zz85 / https://github.com/zz85
 * @author mrdoob / http://mrdoob.com
 * Running this will allow you to drag three.js objects around the screen.
 */
class DLX Drag
{
protected:
  bool _enabled = true;
  three::Raycaster _raycaster;

  three::Camera::Ptr _camera;
  std::vector<three::Object3D::Ptr> _objects;

  three::Object3D *_selected = nullptr, *_hovered = nullptr;

  three::math::Plane _plane;
  std::vector<three::Object3D::Ptr> _surface;
  math::Vector3 _normal;

  void makePlane(const Object3D *object);
  void startSurface(const Intersection &intersect);

  void dragOnPlane();
  void dragOnObjects();

public:
  Signal<void(three::Object3D *)> onDragStarted;
  Signal<void(three::Object3D *)> onDropped;

  Drag(three::Camera::Ptr camera);
  Drag() = default;
  
  bool mouseDown(float x, float y);
  void mouseMove(float x, float y);
  void mouseRelease(float x, float y);

  void setCamera(Camera::Ptr camera)
  {
    _camera = camera;
  }

  void setObjects(const std::vector<three::Object3D::Ptr> &objects)
  {
    _objects = objects;
  }

  void setSurface(const std::vector<three::Object3D::Ptr> &surface)
  {
    _surface = surface;
  }
};

}
}

#endif //THREE_PP_DRAG_H
