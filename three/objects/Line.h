//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_LINE_H
#define THREE_QT_LINE_H

#include <core/Object3D.h>
#include <material/Material.h>

namespace three {

class Line : public Object3D
{
  Geometry::Ptr _geometry;
  Material::Ptr _material;

public:
  Line(Geometry::Ptr geometry, Material::Ptr material) : _geometry(geometry), _material(material) {}

#if 0
  void raycast(raycaster, intersects)
  {
    var inverseMatrix = new Matrix4();
    var ray = new Ray();
    var sphere = new Sphere();

    var precision = raycaster.linePrecision;
    var precisionSq = precision * precision;

    var geometry = this.geometry;
    var matrixWorld = this.matrixWorld;

    // Checking boundingSphere distance to ray

    if ( geometry.boundingSphere === null ) geometry.computeBoundingSphere();

    sphere.copy( geometry.boundingSphere );
    sphere.applyMatrix4( matrixWorld );

    if ( raycaster.ray.intersectsSphere( sphere ) === false ) return;

    //

    inverseMatrix.getInverse( matrixWorld );
    ray.copy( raycaster.ray ).applyMatrix4( inverseMatrix );

    var vStart = new Vector3();
    var vEnd = new Vector3();
    var interSegment = new Vector3();
    var interRay = new Vector3();
    var step = (this && this.isLineSegments) ? 2 : 1;

    if ( geometry.isBufferGeometry ) {

      var index = geometry.index;
      var attributes = geometry.attributes;
      var positions = attributes.position.array;

      if ( index !== null ) {

        var indices = index.array;

        for ( var i = 0, l = indices.length - 1; i < l; i += step ) {

          var a = indices[ i ];
          var b = indices[ i + 1 ];

          vStart.fromArray( positions, a * 3 );
          vEnd.fromArray( positions, b * 3 );

          var distSq = ray.distanceSqToSegment( vStart, vEnd, interRay, interSegment );

          if ( distSq > precisionSq ) continue;

          interRay.applyMatrix4( this.matrixWorld ); //Move back to world space for distance calculation

          var distance = raycaster.ray.origin.distanceTo( interRay );

          if ( distance < raycaster.near || distance > raycaster.far ) continue;

          intersects.push( {

                              distance: distance,
                              // What do we want? intersection point on the ray or on the segment??
                              // point: raycaster.ray.at( distance ),
                              point: interSegment.clone().applyMatrix4( this.matrixWorld ),
                              index: i,
                              face: null,
                              faceIndex: null,
                              object: this

                           } );

        }

      } else {

        for ( var i = 0, l = positions.length / 3 - 1; i < l; i += step ) {

          vStart.fromArray( positions, 3 * i );
          vEnd.fromArray( positions, 3 * i + 3 );

          var distSq = ray.distanceSqToSegment( vStart, vEnd, interRay, interSegment );

          if ( distSq > precisionSq ) continue;

          interRay.applyMatrix4( this.matrixWorld ); //Move back to world space for distance calculation

          var distance = raycaster.ray.origin.distanceTo( interRay );

          if ( distance < raycaster.near || distance > raycaster.far ) continue;

          intersects.push( {

                              distance: distance,
                              // What do we want? intersection point on the ray or on the segment??
                              // point: raycaster.ray.at( distance ),
                              point: interSegment.clone().applyMatrix4( this.matrixWorld ),
                              index: i,
                              face: null,
                              faceIndex: null,
                              object: this

                           } );

        }

      }

    } else if ( geometry.isGeometry ) {

      var vertices = geometry.vertices;
      var nbVertices = vertices.length;

      for ( var i = 0; i < nbVertices - 1; i += step ) {

        var distSq = ray.distanceSqToSegment( vertices[ i ], vertices[ i + 1 ], interRay, interSegment );

        if ( distSq > precisionSq ) continue;

        interRay.applyMatrix4( this.matrixWorld ); //Move back to world space for distance calculation

        var distance = raycaster.ray.origin.distanceTo( interRay );

        if ( distance < raycaster.near || distance > raycaster.far ) continue;

        intersects.push( {

                            distance: distance,
                            // What do we want? intersection point on the ray or on the segment??
                            // point: raycaster.ray.at( distance ),
                            point: interSegment.clone().applyMatrix4( this.matrixWorld ),
                            index: i,
                            face: null,
                            faceIndex: null,
                            object: this

                         } );

      }
    }
  }
#endif
};

}
#endif //THREE_QT_LINE_H
