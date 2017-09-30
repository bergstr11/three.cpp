//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_BOX
#define THREEQT_BOX

#include <core/StaticGeometry.h>
#include <core/BufferGeometry.h>

namespace three {
namespace geometry {

class Box : public StaticGeometry
{
  unsigned const _width, _height, _depth;
  unsigned const _widthSegments, _heightSegments, _depthSegments;

protected:
  Box(unsigned width, unsigned height, unsigned depth,
      unsigned widthSegments, unsigned heightSegments, unsigned depthSegments)
     : _width(width), _height(height), _depth(depth),
       _widthSegments(widthSegments), _heightSegments(heightSegments), _depthSegments(depthSegments) {}

public:
  using Ptr = std::shared_ptr<Box>;
  static Ptr make(unsigned width, unsigned height, unsigned depth, unsigned widthSegments,
                  unsigned heightSegments, unsigned depthSegments) {
    return Ptr(new Box(width, height, depth, widthSegments, heightSegments, depthSegments));
  }
};

class BoxBuffer : public BufferGeometry
{
  unsigned const _width, _height, _depth;
  unsigned _widthSegments, _heightSegments, _depthSegments;

protected:
  BoxBuffer::BoxBuffer(unsigned int width, unsigned int height, unsigned int depth,
                       unsigned widthSegments, unsigned heightSegments, unsigned depthSegments);

public:
  using Ptr = std::shared_ptr<BoxBuffer>;
  static Ptr make(unsigned width, unsigned height, unsigned depth,
                  unsigned widthSegments=1, unsigned heightSegments=1, unsigned depthSegments=1) {
    return Ptr(new BoxBuffer(width, height, depth, widthSegments, heightSegments, depthSegments));
  }
};

}
}


#endif //THREEQT_BOX
