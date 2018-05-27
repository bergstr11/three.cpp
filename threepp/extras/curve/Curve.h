//
// Created by byter on 5/27/18.
//

#ifndef THREE_PP_CURVE_H
#define THREE_PP_CURVE_H

#include <vector>
#include <threepp/math/Vector2.h>
#include <threepp/math/Vector3.h>
#include <threepp/math/Matrix4.h>

namespace three {
namespace extras {

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
protected:
  unsigned arcLengthDivisions = 200;

  std::vector<float> cacheArcLengths;
  bool needsUpdate = false;

public:
  virtual math::Vector2 getPoint(float position) = 0;

  // Get point at relative position in curve according to arc length
  // - u [0 .. 1]
  math::Vector2 getPointAt(float u)
  {
    return getPoint(getUtoTmapping(u));
  }

  // Get sequence of points using getPoint( t )
  std::vector<math::Vector2> getPoints(unsigned divisions = 5)
  {
    std::vector<math::Vector2> points;

    for (unsigned d = 0; d <= divisions; d++) {

      points.push_back(getPoint(d / divisions));
    }

    return points;
  }

  // Get sequence of points using getPointAt( u )
  std::vector<math::Vector2> getSpacedPoints(unsigned divisions = 5)
  {
    std::vector<math::Vector2> points;

    for (unsigned d = 0; d <= divisions; d++) {

      points.push_back(getPointAt(d / divisions));
    }

    return points;
  }

  // Get total curve arc length
  float getLength()
  {
    return getLengths().back();
  }

  // Get list of cumulative segment lengths
  std::vector<float> getLengths(unsigned divisions = arcLengthDivisions)
  {
    if (cacheArcLengths.size() == divisions + 1 && !needsUpdate) {

      return cacheArcLengths;
    }

    needsUpdate = false;

    cacheArcLengths.clear();
    math::Vector2 current, last = getPoint(0);
    float sum = 0;

    cacheArcLengths.push_back(0);

    for (unsigned p = 1; p <= divisions; p++) {

      current = getPoint(p / divisions);
      sum += current.distanceTo(last);
      cacheArcLengths.push_back(sum);
      last = current;
    }

    return cacheArcLengths; // { sums: cache, sum: sum }; Sum is in the last element.
  }

  void updateArcLengths()
  {
    needsUpdate = true;
    getLengths();
  }

  // Given u ( 0 .. 1 ), get a t to find p. This gives you points which are equidistant
  float getUtoTmapping(float u, float distance=0)
  {
    std::vector<float> arcLengths = getLengths();

    float targetArcLength; // The targeted u distance value to get

    if (distance > 0) {
      targetArcLength = distance;
    }
    else {
      targetArcLength = u * arcLengths.back();
    }

    // binary search for the index with largest value smaller than target u distance
    float low = 0, high = arcLengths.size() - 1;
    while (low <= high) {

      float i = low + (high - low) / 2; // less likely to overflow

      float comparison = arcLengths[i] - targetArcLength;

      if (comparison < 0) {

        low = i + 1;
      }
      else if (comparison > 0) {

        high = i - 1;
      }
      else {

        high = i;
        break;
        // DONE
      }
    }

    float i = high;

    if (arcLengths[i] == targetArcLength) {

      return i / (arcLengths.size() - 1);
    }

    // we could get finer grain at lengths, or use simple interpolation between two points
    float lengthBefore = arcLengths[i];
    float lengthAfter = arcLengths[i + 1];

    float segmentLength = lengthAfter - lengthBefore;

    // determine where we are between the 'before' and 'after' points
    float segmentFraction = (targetArcLength - lengthBefore) / segmentLength;

    // add that fractional amount to t
    return (i + segmentFraction) / (arcLengths.size() - 1);
  }

  // Returns a unit vector tangent at t
  // In case any sub curve does not implement its tangent derivation,
  // 2 points a small delta apart will be used to find its gradient
  // which seems to give a reasonable approximation
  math::Vector2 getTangent(float t)
  {
    float delta = 0.0001f;
    float t1 = t - delta;
    float t2 = t + delta;

    // Capping in case of danger
    if (t1 < 0) t1 = 0;
    if (t2 > 1) t2 = 1;

    math::Vector2 pt1 = getPoint(t1);
    math::Vector2 pt2 = getPoint(t2);

    math::Vector2 vec = pt2 - pt1;
    return vec.normalize();
  }

  math::Vector2 getTangentAt(float u)
  {
    return getTangent(getUtoTmapping(u));
  }

  void computeFrenetFrames(unsigned segments, bool closed)
  {
    // see http://www.cs.indiana.edu/pub/techreports/TR425.pdf
    math::Vector3 normal;

    std::vector<math::Vector2> tangents;
    std::vector<math::Vector3> normals;
    std::vector<math::Vector3> binormals;

    /*var vec = new Vector3();
    var mat = new Matrix4();

    var i, u, theta;*/

    // compute the tangent vectors for each segment on the curve
    for (unsigned i = 0; i <= segments; i++) {

      tangents[i] = getTangentAt((float)i / segments).normalize();
    }

    // select an initial normal vector perpendicular to the first tangent vector,
    // and in the direction of the minimum tangent xyz component
    normals.emplace_back(math::Vector3());
    binormals.emplace_back(math::Vector3());
    float min = std::numeric_limits<float>::max();
    float tx = std::abs(tangents[0].x());
    float ty = std::abs(tangents[0].y());
    float tz = 0;

    if (tx <= min) {

      min = tx;
      normal.set(1, 0, 0);

    }

    if (ty <= min) {

      min = ty;
      normal.set(0, 1, 0);
    }

    if (tz <= min) {

      normal.set(0, 0, 1);
    }

    math::Vector3 vec = cross(tangents[0], normal).normalize();

    normals[0] = cross(tangents[0], vec);
    binormals[0] = cross(tangents[0], normals[0]);


    // compute the slowly-varying normal and binormal vectors for each segment on the curve
    for (unsigned i = 1; i <= segments; i++) {

      normals[i] = normals[i - 1];
      binormals[i] = binormals[i - 1];

      math::Vector2 vec = cross(tangents[i - 1], tangents[i]);

      if (vec.length() > std::numeric_limits<float>::epsilon()) {

        vec.normalize();

        float theta = std::acos(math::clamp(tangents[i - 1].dot(tangents[i]), -1, 1)); // clamp for floating pt errors
        normals[i].apply(math::Matrix4::axisRotation(vec, theta));
      }

      binormals[i] = cross(tangents[i], normals[i]);
    }

    // if the curve is closed, postprocess the vectors so the first and last normal vectors are the same
    if (closed) {

      float theta = std::acos(math::clamp(math::dot(normals[0], normals[segments]), -1, 1));
      theta /= segments;

      math::Vector3 t0 = tangents[0];
      if (math::dot(t0, math::cross(normals[0], normals[segments])) > 0) {

        theta = -theta;
      }

      for (unsigned i = 1; i <= segments; i++) {

        // twist a little...
        normals[i].apply(math::Matrix4::axisRotation(tangents[i], theta * i));
        binormals[i] = math::cross(tangents[i], normals[i]);
      }
    }
  }
};

}
}
#endif //THREE_PP_CURVE_H
