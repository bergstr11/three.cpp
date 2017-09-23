//
// Created by byter on 23.09.17.
//

#include "BufferGeometry.h"
#include "Object3D.h"

namespace three {

BufferGeometry::BufferGeometry(Object3D::Ptr object)
{
#if 0
  var geometry = object->geometry();

  if ( object.isPoints || object.isLine ) {

    BufferAttribute<float> positions(geometry.vertices.length * 3, 3 );
    var colors = new Float32BufferAttribute( geometry.colors.length * 3, 3 );

    this.addAttribute( 'position', positions.copyVector3sArray( geometry.vertices ) );
    this.addAttribute( 'color', colors.copyColorsArray( geometry.colors ) );

    if ( geometry.lineDistances && geometry.lineDistances.length === geometry.vertices.length ) {

      var lineDistances = new Float32BufferAttribute( geometry.lineDistances.length, 1 );

      this.addAttribute( 'lineDistance', lineDistances.copyArray( geometry.lineDistances ) );

    }

    if ( geometry.boundingSphere !== null ) {

      this.boundingSphere = geometry.boundingSphere.clone();

    }

    if ( geometry.boundingBox !== null ) {

      this.boundingBox = geometry.boundingBox.clone();

    }

  } else if ( object.isMesh ) {

    if ( geometry && geometry.isGeometry ) {

      this.fromGeometry( geometry );

    }

  }
#endif
}

BufferGeometry &BufferGeometry::update(std::shared_ptr<Object3D> object)
{
#if 0
  var geometry = object.geometry;

  if ( object.isMesh ) {

    var direct = geometry.__directGeometry;

    if ( geometry.elementsNeedUpdate === true ) {

      direct = undefined;
      geometry.elementsNeedUpdate = false;

    }

    if ( direct === undefined ) {

      return this.fromGeometry( geometry );

    }

    direct.verticesNeedUpdate = geometry.verticesNeedUpdate;
    direct.normalsNeedUpdate = geometry.normalsNeedUpdate;
    direct.colorsNeedUpdate = geometry.colorsNeedUpdate;
    direct.uvsNeedUpdate = geometry.uvsNeedUpdate;
    direct.groupsNeedUpdate = geometry.groupsNeedUpdate;

    geometry.verticesNeedUpdate = false;
    geometry.normalsNeedUpdate = false;
    geometry.colorsNeedUpdate = false;
    geometry.uvsNeedUpdate = false;
    geometry.groupsNeedUpdate = false;

    geometry = direct;

  }

  var attribute;

  if ( geometry.verticesNeedUpdate === true ) {

    attribute = this.attributes.position;

    if ( attribute !== undefined ) {

      attribute.copyVector3sArray( geometry.vertices );
      attribute.needsUpdate = true;

    }

    geometry.verticesNeedUpdate = false;

  }

  if ( geometry.normalsNeedUpdate === true ) {

    attribute = this.attributes.normal;

    if ( attribute !== undefined ) {

      attribute.copyVector3sArray( geometry.normals );
      attribute.needsUpdate = true;

    }

    geometry.normalsNeedUpdate = false;

  }

  if ( geometry.colorsNeedUpdate === true ) {

    attribute = this.attributes.color;

    if ( attribute !== undefined ) {

      attribute.copyColorsArray( geometry.colors );
      attribute.needsUpdate = true;

    }

    geometry.colorsNeedUpdate = false;

  }

  if ( geometry.uvsNeedUpdate ) {

    attribute = this.attributes.uv;

    if ( attribute !== undefined ) {

      attribute.copyVector2sArray( geometry.uvs );
      attribute.needsUpdate = true;

    }

    geometry.uvsNeedUpdate = false;

  }

  if ( geometry.lineDistancesNeedUpdate ) {

    attribute = this.attributes.lineDistance;

    if ( attribute !== undefined ) {

      attribute.copyArray( geometry.lineDistances );
      attribute.needsUpdate = true;

    }

    geometry.lineDistancesNeedUpdate = false;

  }

  if ( geometry.groupsNeedUpdate ) {

    geometry.computeGroups( object.geometry );
    this.groups = geometry.groups;

    geometry.groupsNeedUpdate = false;

  }
#endif
  return *this;
}

}
