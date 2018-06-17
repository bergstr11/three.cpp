//
// Created by byter on 6/15/18.
//

#include "Curve.h"

namespace three {
namespace extras {

using namespace math;

// Get list of cumulative segment lengths
const std::vector<float> &Curve::getLengths(unsigned divisions)
{
  if (_cacheArcLengths.size() == divisions + 1 && !_needsUpdate) {

    return _cacheArcLengths;
  }

  _needsUpdate = false;

  _cacheArcLengths.clear();
  Vector2 current, last = getPoint(0);
  float sum = 0;

  _cacheArcLengths.push_back(0);

  for (unsigned p = 1; p <= divisions; p++) {

    current = getPoint(p / divisions);
    sum += current.distanceTo(last);
    _cacheArcLengths.push_back(sum);
    last = current;
  }

  return _cacheArcLengths; // { sums: cache, sum: sum }; Sum is in the last element.
}

float Curve::getUtoTmapping(float u, float distance)
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

Vector2 Curve::getTangent(float t)
{
  float delta = 0.0001f;
  float t1 = t - delta;
  float t2 = t + delta;

  // Capping in case of danger
  if (t1 < 0) t1 = 0;
  if (t2 > 1) t2 = 1;

  Vector2 pt1 = getPoint(t1);
  Vector2 pt2 = getPoint(t2);

  Vector2 vec = pt2 - pt1;
  return vec.normalize();
}

void Curve::computeFrenetFrames(unsigned segments, bool closed,
                                std::vector<Vector3> &tangents,
                                std::vector<Vector3> &normals,
                                std::vector<Vector3> &binormals)
{
  // see http://www.cs.indiana.edu/pub/techreports/TR425.pdf

  // compute the tangent vectors for each segment on the curve
  for (unsigned i = 0; i <= segments; i++) {

    tangents[i] = getTangentAt((float)i / segments).normalize();
  }

  // select an initial normal vector perpendicular to the first tangent vector,
  // and in the direction of the minimum tangent xyz component
  normals.emplace_back(Vector3());
  binormals.emplace_back(Vector3());
  float min = std::numeric_limits<float>::max();
  float tx = std::abs(tangents[0].x());
  float ty = std::abs(tangents[0].y());
  float tz = 0;

  Vector3 normal;
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

  Vector3 vec = cross(tangents[0], normal).normalize();

  normals[0] = cross(tangents[0], vec);
  binormals[0] = cross(tangents[0], normals[0]);


  // compute the slowly-varying normal and binormal vectors for each segment on the curve
  for (unsigned i = 1; i <= segments; i++) {

    normals[i] = normals[i - 1];
    binormals[i] = binormals[i - 1];

    vec = cross(tangents[i - 1], tangents[i]);

    if (vec.length() > std::numeric_limits<float>::epsilon()) {

      vec.normalize();

      float theta = std::acos(clamp(dot(tangents[i - 1], tangents[i]), -1.0f, 1.0f)); // clamp for floating pt errors
      normals[i].apply(Matrix4::axisRotation(vec, theta));
    }

    binormals[i] = cross(tangents[i], normals[i]);
  }

  // if the curve is closed, postprocess the vectors so the first and last normal vectors are the same
  if (closed) {

    float theta = std::acos(clamp(dot(normals[0], normals[segments]), -1.0f, 1.0f));
    theta /= segments;

    Vector3 t0 = tangents[0];
    if (dot(t0, cross(normals[0], normals[segments])) > 0) {

      theta = -theta;
    }

    for (unsigned i = 1; i <= segments; i++) {

      // twist a little...
      normals[i].apply(Matrix4::axisRotation(tangents[i], theta * i));
      binormals[i] = cross(tangents[i], normals[i]);
    }
  }
}

}
}