//
// Created by byter on 5/27/18.
//

#ifndef THREE_PP_EXTRUDE_H
#define THREE_PP_EXTRUDE_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

struct ExtrudeOptions
{
  unsigned curveSegments = 12;
  unsigned steps = 1;
  unsigned amount = 100;
  bool bevelEnabled = true;
  unsigned bevelThickness = 6;
  unsigned bevelSize = bevelThickness - 2;
  unsigned bevelSegments = 3;
  //extrudePath;
  //var uvgen = WorldUVGenerator;
};

/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 *
 * Creates extruded geometry from a path shape.
 *
 * parameters = {
 *
 *  curveSegments: <int>, // number of points on the curves
 *  steps: <int>, // number of points for z-side extrusions / used for subdividing segments of extrude spline too
 *  amount: <float>, // Depth to extrude the shape
 *
 *  bevelEnabled: <bool>, // turn on bevel
 *  bevelThickness: <float>, // how deep into the original shape bevel goes
 *  bevelSize: <float>, // how far from shape outline is bevel
 *  bevelSegments: <int>, // number of bevel layers
 *
 *  extrudePath: <THREE.Curve> // curve to extrude shape along
 *
 *  UVGenerator: <Object> // object that provides UV generator functions
 *
 * }
 */
class Extrude : public LinearGeometry
{
protected:
  Extrude(const ExtrudeOptions &options);
};

namespace buffer {

class Extrude : public BufferGeometry
{
protected:
  Extrude(const ExtrudeOptions &options);
};

}

}
}


#endif //THREE_PP_EXTRUDE_H
