//
// Created by byter on 04.09.17.
//

#ifndef THREE_QT_RAYCASTER_H
#define THREE_QT_RAYCASTER_H

#include <math/Ray.h>
//#include <camera/Camera.h>

namespace three {

#if 0
class Raycaster;
void intersectObject(Object3D &object, const Raycaster &raycaster, intersects, bool recursive ) {

  if (!object.visible()) return;

  object.raycast( raycaster, intersects );

  if ( recursive === true ) {
    var children = object.children;

    for ( var i = 0, l = children.length; i < l; i ++ ) {
      intersectObject( children[ i ], raycaster, intersects, true );
    }
  }
}
#endif

class Raycaster
{
  math::Ray _ray;
  float _near, _far;

  float _linePrecision = 1;

public:
  // direction is assumed to be normalized (for accurate distance calculations)
  Raycaster(const math::Vector3 &origin, const math::Vector3 &direction, float near=0, float far=std::numeric_limits<float>::infinity())
     : _ray(origin, direction), _near(near), _far(far) {}

  Raycaster &set(const math::Vector3 &origin, const math::Vector3 &direction )
  {
    // direction is assumed to be normalized (for accurate distance calculations)
    _ray.set( origin, direction );
  }

  float linePrecision() const {return _linePrecision;}
  const math::Ray &ray() const {return _ray;}
  const float near() const {return _near;}
  const float far() const {return _far;}

#if 0
  Raycaster &setFromCamera(const math::Vector3 &coords, const Camera &camera)
  {
    if ((camera && camera.isPerspectiveCamera ) ) {

      _ray.origin() = math::Vector3::fromMatrixPosition(camera.matrixWorld());
      _ray.direction().set( coords.x(), coords.y(), 0.5 ).unproject( camera );
      _ray.direction() -= _ray.origin();
      _ray.direction().normalize();

    }
    else if (camera.isOrthographicCamera) {

      _ray.origin().set( coords.x, coords.y, ( camera.near() + camera.far() ) / ( camera.near() - camera.far() ) ).unproject( camera ); // set origin in plane of camera
      _ray.direction().set( 0, 0, - 1 ).transformDirection( camera.matrixWorld() );
    }
    else {
      console.error( 'THREE.Raycaster: Unsupported camera type.' );
    }
  }

  intersectObject( object, recursive ) {

    var intersects = [];

    intersectObject( object, this, intersects, recursive );

    intersects.sort( ascSort );

    return intersects;
  }

  intersectObjects: function ( objects, recursive ) {

    var intersects = [];

    if ( Array.isArray( objects ) === false ) {

      console.warn( 'THREE.Raycaster.intersectObjects: objects is not an Array.' );
      return intersects;

    }

    for ( var i = 0, l = objects.length; i < l; i ++ ) {

      intersectObject( objects[ i ], this, intersects, recursive );

    }

    intersects.sort( ascSort );

    return intersects;

  }
#endif
};

}
#endif //THREE_QT_RAYCASTER_H
