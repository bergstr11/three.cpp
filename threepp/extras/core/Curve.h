//
// Created by byter on 5/27/18.
//

#ifndef THREE_PP_CURVE_H
#define THREE_PP_CURVE_H

#include <vector>
#include <memory>
#include <threepp/math/Vector2.h>
#include <threepp/math/Vector3.h>
#include <threepp/math/Matrix4.h>

namespace three {
namespace extras {

class CurvePath;

/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 * Extensible curve object, Abstract Curve base class
 *
 * Some common of curve methods:
 * .getPoint( t, optionalTarget ), .getTangent( t )
 * .getPointAt( u, optionalTarget ), .getTangentAt( u )
 * .getPoints(), .getSpacedPoints()
 * .getLength()
 * .updateArcLengths()
 *
 * This following curves inherit from THREE.Curve:
 *
 * -- 2D curves --
 * THREE.ArcCurve
 * THREE.CubicBezierCurve
 * THREE.EllipseCurve
 * THREE.LineCurve
 * THREE.QuadraticBezierCurve
 * THREE.SplineCurve
 *
 * -- 3D curves --
 * THREE.CatmullRomCurve3
 * THREE.CubicBezierCurve3
 * THREE.LineCurve3
 * THREE.QuadraticBezierCurve3
 *
 * A series of curves can be represented as a THREE.CurvePath.
 *
 **/
class Curve
{
  friend class CurvePath;

protected:
  std::vector<float> _cacheArcLengths;
  bool _needsUpdate = false;

  virtual unsigned pathResolution(unsigned divisions) const {
    //curve.isSplineCurve) ? divisions * curve.points.length
    return divisions;
  }

public:
  static const unsigned arcLengthDivisions = 200;

  using Ptr = std::shared_ptr<Curve>;

  virtual ~Curve() = default;

  virtual math::Vector2 getPoint(float position) = 0;

  // Get point at relative position in curve according to arc length
  // - u [0 .. 1]
  virtual math::Vector2 getPointAt(float u)
  {
    return getPoint(getUtoTmapping(u));
  }

  // Get sequence of points using getPoint( t )
  virtual void getPoints(unsigned divisions, std::vector<math::Vector2> &points)
  {
    for (unsigned d = 0; d <= divisions; d++) {

      points.push_back(getPoint(d / divisions));
    }
  }

  virtual std::vector<math::Vector2> getPoints()
  {
    std::vector<math::Vector2> points;
    getPoints(5, points);
    return points;
  }

  // Get sequence of points using getPointAt( u )
  virtual void getSpacedPoints(unsigned divisions, std::vector<math::Vector2> &points)
  {
    for (unsigned d = 0; d <= divisions; d++) {

      points.push_back(getPointAt(d / divisions));
    }
  }

  // Get total curve arc length
  virtual float getLength()
  {
    return getLengths().back();
  }

  // Get list of cumulative segment lengths
  const std::vector<float> &getLengths(unsigned divisions = arcLengthDivisions);

  virtual void updateArcLengths()
  {
    _needsUpdate = true;
    getLengths();
  }

  // Given u ( 0 .. 1 ), get a t to find p. This gives you points which are equidistant
  float getUtoTmapping(float u, float distance=0);

  // Returns a unit vector tangent at t
  // In case any sub curve does not implement its tangent derivation,
  // 2 points a small delta apart will be used to find its gradient
  // which seems to give a reasonable approximation
  virtual math::Vector2 getTangent(float t);

  math::Vector2 getTangentAt(float u)
  {
    return getTangent(getUtoTmapping(u));
  }

  void computeFrenetFrames(unsigned segments, bool closed,
                           std::vector<math::Vector3> &tangents,
                           std::vector<math::Vector3> &normals,
                           std::vector<math::Vector3> &binormals);
};

}
}
#endif //THREE_PP_CURVE_H
