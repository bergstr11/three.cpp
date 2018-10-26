//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_GEOM_BOX_H
#define THREEPP_GEOM_BOX_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

class BoxParams
{
protected:
  float _width, _height, _depth;
  unsigned _widthSegments, _heightSegments, _depthSegments;

  BoxParams(float width, float height, float depth, unsigned widthSegments, unsigned heightSegments, unsigned depthSegments)
     : _width(width), _height(height), _depth(depth), _widthSegments(widthSegments), _heightSegments(heightSegments),
       _depthSegments(depthSegments)
  {}

public:
  void setWidth(float width) {_width = width;}
  void setHeight(float height) {_height = height;}
  void setDepth(float depth) {_depth = depth;}
};

class DLX Box : public LinearGeometry, public BoxParams
{
protected:
  Box(const Box &box);

  Box(float width, float height, float depth,
      unsigned widthSegments, unsigned heightSegments, unsigned depthSegments);

public:
  using Ptr = std::shared_ptr<Box>;
  static Ptr make(float width, float height, float depth,
                  unsigned widthSegments=1,
                  unsigned heightSegments=1,
                  unsigned depthSegments=1) {
    return Ptr(new Box(width, height, depth, widthSegments, heightSegments, depthSegments));
  }

  Box *cloned() const override {
    return new Box(*this);
  }
};

namespace buffer {

class DLX Box : public BufferGeometry, public BoxParams
{
  friend class three::geometry::Box;

protected:
  Box(const Box &box);

  Box(float width, float height, float depth,
      unsigned widthSegments, unsigned heightSegments, unsigned depthSegments);

public:
  using Ptr = std::shared_ptr<Box>;
  static Ptr make(float width, float height, float depth,
                  unsigned widthSegments=1, unsigned heightSegments=1, unsigned depthSegments=1) {
    return Ptr(new Box(width, height, depth, widthSegments, heightSegments, depthSegments));
  }

  Box *cloned() const override {
    return new Box(*this);
  }
};

}

}
}


#endif //THREEPP_GEOM_BOX_H
