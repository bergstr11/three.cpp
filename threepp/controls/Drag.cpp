//
// Created by byter on 8/18/18.
//

#include "Drag.h"

namespace three {
namespace control {

Drag::Drag(three::Camera::Ptr camera)
   : _camera(camera)
{}

bool Drag::mouseDown(float x, float y)
{
  _selected = nullptr;
  _raycaster.set(_camera->ray(x, y));

  IntersectList intersects;
  _raycaster.intersectObjects(_objects, intersects, false);

  if(!intersects.empty()) {

    const auto &intersect = (*intersects.begin());
    _selected = intersect.object;

    if(_surface.empty())
      makePlane(_selected);
    else
      startSurface(intersect);

    onDragStarted.emitSignal(_selected);
    return true;
  }
  return false;
}

void Drag::mouseMove(float x, float y)
{
  _raycaster.set( _camera->ray(x, y) );

  if ( _selected && _enabled ) {

    if(_surface.empty())
      dragOnPlane();
    else
      dragOnObjects();

    return;
  }
  else {
    //only entered if hover events are enabled, i.e. in Qt, setMouseTracking is on
    IntersectList intersects;
    _raycaster.intersectObjects( _objects, intersects );

    if ( !intersects.empty() ) {

      auto object = (*intersects.begin()).object;

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

void Drag::startSurface(const Intersection &intersect)
{
  IntersectList intersects;
  _raycaster.intersectObjects(_surface, intersects, false);
  if(!intersects.empty())
    _normal = (*intersects.begin()).face.normal;
  else
    _normal = intersect.face.normal;
}

void Drag::makePlane(const Object3D *object)
{
  _plane = math::Plane::fromNormalAndCoplanarPoint(_camera->getWorldDirection(), _selected->position());

  //translate the plane to the position of the selected object. Why it's not created there
  //in the first place by fromNormalAndCoplanarPoint - don't know
  math::Vector3 intersect;
  _raycaster.centerRay().intersectPlane(_plane, intersect);
  _plane.translate((intersect - object->position()) * -1);
}

void Drag::dragOnPlane()
{
  math::Vector3 intersection;
  if (_raycaster.centerRay().intersectPlane(_plane, intersection)) {

    _selected->position() = intersection;
  }
}

void Drag::dragOnObjects()
{
  IntersectList intersects;
  _raycaster.intersectObjects(_surface, intersects, false);

  if(!intersects.empty()) {

    const auto &intersect = *intersects.begin();
    _selected->position() = intersect.object->worldToLocal(intersect.point);

    auto faceNormal = _selected->worldToLocal(intersect.face.normal);
    math::Vector3 axis = math::cross(_selected->worldToLocal(_normal), faceNormal).normalized();
    float angle = intersect.face.normal.angleTo(_normal);

    _selected->rotateOnAxis(axis.negated(), angle);

    _normal = intersect.face.normal;
  }
}


}
}
