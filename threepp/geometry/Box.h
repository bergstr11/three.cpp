//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_GEOM_BOX_H
#define THREEPP_GEOM_BOX_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

class DLX Box : public LinearGeometry
{
  unsigned _width, _height, _depth;
  unsigned _widthSegments, _heightSegments, _depthSegments;

protected:
  Box(unsigned width, unsigned height, unsigned depth,
      unsigned widthSegments, unsigned heightSegments, unsigned depthSegments);

public:
  using Ptr = std::shared_ptr<Box>;
  static Ptr make(unsigned width, unsigned height, unsigned depth,
                  unsigned widthSegments=1,
                  unsigned heightSegments=1,
                  unsigned depthSegments=1) {
    return Ptr(new Box(width, height, depth, widthSegments, heightSegments, depthSegments));
  }

  Box *cloned() const override {
    return LinearGeometry::setTyper(new Box(*this));
  }

  void setWidth(unsigned width) {_width = width;}
  void setHeight(unsigned height) {_height = height;}
  void setDepth(unsigned depth) {_depth = depth;}
};

namespace buffer {

class DLX Box : public BufferGeometry
{
  friend class three::geometry::Box;

  unsigned const _width, _height, _depth;
  unsigned _widthSegments, _heightSegments, _depthSegments;

protected:
  Box(unsigned int width, unsigned int height, unsigned int depth, unsigned widthSegments,
            unsigned heightSegments, unsigned depthSegments);

public:
  using Ptr = std::shared_ptr<Box>;
  static Ptr make(unsigned width, unsigned height, unsigned depth,
                  unsigned widthSegments=1, unsigned heightSegments=1, unsigned depthSegments=1) {
    return Ptr(new Box(width, height, depth, widthSegments, heightSegments, depthSegments));
  }

  Box *cloned() const override {
    return BufferGeometry::setTyper(new Box(*this));
  }
};

}

}
}


#endif //THREEPP_GEOM_BOX_H
