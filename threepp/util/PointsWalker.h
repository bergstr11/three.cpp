//
// Created by byter on 5/6/18.
//

#ifndef THREE_PP_POINTSWALKER_H
#define THREE_PP_POINTSWALKER_H

#include <threepp/core/Object3D.h>
#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {

/**
 * an iterator that will walk object vertices recursively in an object tree, transforming
 * to world coordinates on the fly
 */
struct PointsWalker
{
  using Ptr = std::shared_ptr<PointsWalker>;

private:
  using child_iterator = std::vector<Object3D::Ptr>::const_iterator;

  Object3D::Ptr _object;
  const Vertex *_data;
  size_t _dataCount = 0;

  child_iterator _beginChildren;
  child_iterator _endChildren;
  Ptr _childWalker;

  PointsWalker(Object3D::Ptr object, const Vertex *data, size_t dataCount)
     : _object(object),
       _beginChildren(object->children().begin()),
       _endChildren(object->children().end()),
       _data(data),
       _dataCount(dataCount),
       _childWalker(nullptr)
  {
    object->updateMatrixWorld( true );
  }

  PointsWalker(Object3D::Ptr object, const child_iterator &beginChildren, Ptr walker)
     : _object(object),
       _beginChildren(beginChildren),
       _endChildren(object->children().end()),
       _data(nullptr),
       _dataCount(0),
       _childWalker(walker)
  {
    object->updateMatrixWorld( true );
  }

public:
  PointsWalker(const child_iterator &end)
     : _object(nullptr),
       _endChildren(end),
       _data(nullptr),
       _dataCount(0),
       _childWalker(nullptr)
  {}

  static Ptr make(const Object3D::Ptr &object)
  {
    Geometry::Ptr geometry = object->geometry();
    const Vertex *data = nullptr;
    size_t dataCount = 0;

    if(geometry) {
      if(LinearGeometry *lin = geometry->typer) {
        data = lin->vertices().data();
        dataCount = lin->vertices().size();
      }
      else if(BufferGeometry *buf = geometry->typer) {
        data = buf->position()->data<Vertex>();
        dataCount = buf->position()->itemCount();
      }
      else {
        throw std::invalid_argument("unsupported geometry type");
      }
    }
    if(dataCount) {
      return Ptr(new PointsWalker(object, data, dataCount));
    }
    else {
      auto beginChildren = object->children().begin();
      auto endChildren = object->children().end();
      for(; beginChildren != endChildren; beginChildren++) {
        Ptr ptr = make(*beginChildren);
        if(ptr) return Ptr(new PointsWalker(object, beginChildren, ptr));
      }
    }
    return nullptr;
  }

  const Vertex operator*() const
  {
    Vertex value = *_data;
    return value.apply(_object->matrixWorld());
  }

  PointsWalker &operator++() {
    if(_dataCount > 0) {
      _dataCount--;
      _data++;
    }
    else if(_childWalker && !_childWalker->atEnd()) {
      ++(*_childWalker);
    }
    else {
      for(; _beginChildren != _endChildren; _beginChildren++) {
        _childWalker = make(*_beginChildren);
        if(_childWalker) break;
      }
    }
    return *this;
  }

  bool operator==(const PointsWalker& rhs) const
  {
    return _dataCount == rhs._dataCount  &&
           _beginChildren == rhs._beginChildren;
  }

  bool operator!=(const PointsWalker& rhs) const
  {
    return !(*this == rhs);
  }

  bool atEnd() {
    return _dataCount == 0 && _beginChildren == _endChildren;
  }
};

}

#endif //THREE_PP_POINTSWALKER_H
