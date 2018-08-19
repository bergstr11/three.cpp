//
// Created by byter on 8/18/18.
//

#include "Drag.h"

namespace three {
namespace control {

Drag::Drag(three::Camera::Ptr camera, const std::vector<three::Object3D::Ptr> &objects)
   : _camera(camera), _objects(objects)
{}

void Drag::makePlane(const Object3D *object)
{
  _plane = math::Plane::fromNormalAndCoplanarPoint(_camera->getWorldDirection(), _selected->position());

  //translate the plane to the position of the selected object. Why it's not created there
  //in the first place by fromNormalAndCoplanarPoint - don't know
  math::Vector3 intersect;
  _raycaster.centerRay().intersectPlane(_plane, intersect);
  _plane.translate((intersect - object->position()) * -1);
}

bool Drag::mouseDown(float x, float y)
{
  _selected = nullptr;
  _raycaster.set(_camera->ray(x, y));

  IntersectList intersects;
  _raycaster.intersectObjects(_objects, intersects);

  if(!intersects.empty()) {
    _selected  =  (*intersects.begin()).object;
    makePlane(_selected);

    onDragStarted.emitSignal(_selected);
    return true;
  }
  return false;
}

void Drag::mouseMove(float x, float y)
{
  _raycaster.set( _camera->ray(x, y) );

  if ( _selected && _enabled ) {

    math::Vector3 intersection;
    if ( _raycaster.centerRay().intersectPlane( _plane, intersection ) ) {

      _selected->position() = intersection;

      auto d = _camera->position().distanceTo(_selected->position());
    }
    return;
  }
  else {
    //only entered if hover events are enabled, i.e. in Qt, setMouseTracking is on
    IntersectList intersects;
    _raycaster.intersectObjects( _objects, intersects );

    if ( !intersects.empty() ) {

      auto object = (*intersects.begin()).object;
      makePlane(object);

      if ( _hovered != object ) {

        _hovered = object;
      }

    } else {

      if ( _hovered ) {

        _hovered = nullptr;
      }
    }
  }
}

void Drag::mouseRelease(float x, float y)
{
  if(_selected) {
    onDropped.emitSignal(_selected);
  }
}

}
}
