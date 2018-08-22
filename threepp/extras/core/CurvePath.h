//
// Created by byter on 6/15/18.
//

#ifndef THREE_PP_CURVEPATH_H
#define THREE_PP_CURVEPATH_H

#include "Curve.h"
#include <vector>

namespace three {
namespace extras {

/**
 *	Curved Path - a curve path is simply a array of connected
 *  curves, but retains the api of a curve
 *
 * @author zz85 / http://www.lab4games.net/zz85/blog
 */
class CurvePath : public Curve
{
protected:
  std::vector<Curve::Ptr> _curves;

  std::vector<float> _cacheLengths;

  bool _autoClose = false;

  void closePath();

  explicit CurvePath(const std::vector<Curve::Ptr> &curves) : _curves(curves) {}

  CurvePath() = default;

public:
  CurvePath &push_back(const Curve::Ptr &curve)
  {
    _curves.push_back(curve);
    return *this;
  }

  const std::vector<Curve::Ptr> &curves() {return _curves;}

  const Curve::Ptr curve(unsigned index) {return _curves.at(index);}

  // To get accurate point with reference to
  // entire path distance at time t,
  // following has to be done:

  // 1. Length of each sub path have to be known
  // 2. Locate and identify type of curve
  // 3. Get t for the curve
  // 4. Return curve.getPointAt(t')

  math::Vector2 getPoint( float t ) override;

  bool autoClose() const {return _autoClose;}

  void setAutoClose(bool autoclose) {
    _autoClose = autoclose;
  }

  // We cannot use the default THREE.Curve getPoint() with getLength() because in
  // THREE.Curve, getLength() depends on getPoint() but in THREE.CurvePath
  // getPoint() depends on getLength

  float getLength() override
  {
    return getCurveLengths().back();
  }

  // cacheLengths must be recalculated.
  void updateArcLengths() override
  {
    _needsUpdate = true;
    _cacheLengths.clear();
    getCurveLengths();
  }

  // Compute lengths and cache them
  // We cannot overwrite getLengths() because UtoT mapping uses it.
  const std::vector<float> &getCurveLengths()
  {
    // We use cache values if curves and cache array are same length
    if ( _cacheLengths.size() == _curves.size()) {

      return _cacheLengths;
    }

    // Get length of sub-curve
    // Push sums into cached array
    _cacheLengths.clear();
    float sum = 0;

    for (unsigned i = 0, l = _curves.size(); i < l; i ++ ) {

      sum += _curves[ i ]->getLength();
      _cacheLengths.push_back( sum );
    }

    return _cacheLengths;
  }

  void getSpacedPoints(unsigned divisions, std::vector<math::Vector2> &points) override;

  void getPoints(unsigned divisions, std::vector<math::Vector2>  &points) override;

  std::vector<math::Vector2> getPoints() override
  {
    std::vector<math::Vector2> points;
    getPoints(12, points);
    return points;
  }

  math::Vector2 start() const override {
    return _curves[0]->start();
  }
};

}
}

#endif //THREE_PP_CURVEPATH_H
